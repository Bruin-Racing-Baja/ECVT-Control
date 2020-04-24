/* m20_actuator_test.cpp
 *
 * Tests the assembled BERTHA actuator and peripherals
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 23 April 2020
 */

#include <Arduino.h>
#include <ODriveArduino.h>
#include <TimerOne.h>

// printing with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }
char endl = '\n';

// odrive interface
#define odrv_serial Serial1 // MISO = ODRV1, TEENSY0; MOSI = ODRV2, TEENSY1
ODriveArduino odrv(odrv_serial);
const byte odrv_rst = 4;

// select motor axis
const byte axis = 1;

// other sensors
const byte act_pot = 14;    // actuator potentiometer
const byte SLS1 = 2;        // software limit switch 1
const byte SLS2 = 3;        // software limit switch 2

// controller
const int ts = 10e3; // [us]
unsigned long t(0);
int apot(0);        // actuator potentiometer reading
double menck(0);    // motor encoder counts at timestep k
double menck1(0);   // motor encoder counts at timestep k-1
double mvelk(0);    // motor velocity at timestep k
const int u_min = -600;
const int u_max = 600;
double u_k(0);      // motor control signal

// function declarations
void control_function();

void setup() {
    
    // initialize computer serial
    Serial.begin(115200);
    while (!Serial);
    
    // configure peripherals
    analogReadRes(10); // 10 bit ADC conversion
    pinMode(act_pot, INPUT);

    // reset odrive to clear watchdog status
    pinMode(odrv_rst, OUTPUT);
    digitalWriteFast(4, LOW);
    delay(100);
    digitalWriteFast(4, HIGH);

    // initialize odrive serial
    odrv_serial.begin(115200);

    // odrive closed loop control mode, start zero velocity
    odrv.run_state(axis, ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL, false);
    odrv.SetVelocity(axis, 0);

    // configure interrupts
    Timer1.initialize(ts);
    Timer1.attachInterrupt(control_function);

}

void loop() {
}

void control_function() {

    // get current time
    t = micros();

    // get current position
    apot = analogRead(act_pot);
    odrv_serial << "r axis" << axis << ".encoder.pos_estimate" << endl;
    menck = odrv.readFloat();

    // calculate encoder velocity
    mvelk = (menck - menck1)*1e6/ts; // [count/s]

    // calculate control signal
    u_k = 0;

    // unidirectional limit switching
    if (digitalReadFast(SLS1) == LOW) { // outer limit reached
        u_k = constrain(u_k, u_min, 0);
    }
    if (digitalReadFast(SLS2) == LOW) { // inner limit reached
        u_k = constrain(u_k, 0, u_max);
    }

    // write control signal
    odrv.SetVelocity(axis, u_k);

    // update past values
    menck1 = menck;

    // print input/output data with timestamp
    Serial
        << t/1e6 << ','
        << apot << ','
        << mvelk << ','
        << u_k << endl;

}