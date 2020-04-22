/* m20_actuator_sid.ino
 *
 * Gathers response data for a system identification of the BERTHA actuator.
 * The input is a velocity command and the output is a velocity calculated
 * from encoder position readings. Readings are printed to serial for live
 * interpretation by m20_actuator_sid.m. Intended for use with a Teensy 3.2 or
 * above to achieve the necessary sample rate.
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 19 March 2020
 */

#include <TimerOne.h>
#include <ODriveArduino.h>
#include <signals.h>

// printing with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }
char endl = '\n';

// odrive interface
#define odrv_serial Serial1
ODriveArduino odrv(odrv_serial);
const byte odrv_rst = 4;
const byte axis = 1;

// controller
unsigned long t(0);
const int ts = 10e3; // [us]
int pos_ref = 100; // [counts]
const double Kp = 10;
const double Ki = 0.03;
double pos_k(0);
double pos_k1(0);
double vel_k(0);
double e_k(0);
double se_k(0);
double u_k(0);

void setup() {

    // initialize computer serial
    Serial.begin(115200);
    while (!Serial);

    delay(1000);

    // reset odrive to clear watchdog status
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    delay(100);
    digitalWrite(4, HIGH);

    // initialize odrive serial
    odrv_serial.begin(115200);

    // setup odrive
    odrv.run_state(axis, ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL, false); // closed loop control mode
    odrv.SetVelocity(axis, 0); // write zero velocity

    // initialize interrupts
    Timer1.initialize(ts);
    Timer1.attachInterrupt(control_function);
    interrupts();

    int amp = 200;
    double start_freq = 0.1*2*PI;
    double stop_freq = 10*2*PI;
    int per = 10;
    int offset = 400;
    configure_chirp(amp, start_freq, stop_freq, per, offset);

}

void loop() {
    
}

void control_function() {

    // get current time
    t = micros();

    // get current position
    odrv_serial << "r axis" << axis << ".encoder.pos_estimate" << endl;
    pos_k = odrv.readFloat();

    // calculate current velocity
    vel_k = (pos_k - pos_k1)*1e6/ts;

    // calculate control signal
    u_k = chirp(t);

    // write control signal
    odrv.SetVelocity(axis, u_k);

    // update past values
    pos_k1 = pos_k;

    // print input/output data with timestamp
    Serial << t/1e6 << ',' << u_k << ',' << vel_k << endl;
}