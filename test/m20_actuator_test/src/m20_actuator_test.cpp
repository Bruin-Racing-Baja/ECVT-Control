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
const byte SLS1 = 11;       // software limit switch 1
const byte SLS2 = 12;       // software limit switch 2

// encoder soft limits
int enc_max = 0;
int enc_min = -1 + 2^15;
bool hit_max(false);
bool hit_min(false);

// controller
const int ts = 10e3; // [us]
unsigned long t(0);
int apot(0);        // actuator potentiometer reading
double menck(0);    // motor encoder counts at timestep k
double menck1(0);   // motor encoder counts at timestep k-1
double mvelk(0);    // motor velocity at timestep k
const int u_abs_min = -600;
const int u_abs_max = 600;
int u_min = -600;
int u_max = 600;
double u_k(0);      // motor control signal

// direction switch on button
const byte dir_button = 6;
int dir = 1;
unsigned long last_trigger(0);

// function declarations
void setup();
int homing_routine();
void switch_direction();
void control_function();
void loop();

void setup() {

    // initialize odrive serial
    odrv_serial.begin(115200);

    // check bus voltage
    double vbus = 0.;
    while (vbus < 20) {
        odrv_serial << "r vbus_voltage" << endl;
        vbus = odrv.readFloat();
        // Serial << "Bus voltage: " << vbus << endl;
    }
    
    // initialize computer serial
    Serial.begin(115200);
    while (!Serial);
    Serial.println("M20 ACTUATOR TEST");
    
    // configure peripherals
    analogReadRes(10); // 10 bit ADC conversion
    pinMode(act_pot, INPUT);
    pinMode(dir_button, INPUT_PULLUP);

    // reset odrive to clear watchdog status
    pinMode(odrv_rst, OUTPUT);
    digitalWriteFast(odrv_rst, LOW);
    delay(100);
    digitalWriteFast(odrv_rst, HIGH);

    // odrive closed loop control mode, start zero velocity
    odrv.run_state(axis, ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL, false);
    odrv.SetVelocity(axis, 0);

    // find soft limits
    homing_routine();

    // configure interrupts
    attachInterrupt(dir_button, switch_direction, RISING);
    Timer1.initialize(ts);
    Timer1.attachInterrupt(control_function);

}

int homing_routine() {

    // set velocity to zero
    odrv.SetVelocity(axis, 0);

    // find outer limit
    Serial.println("finding outer limit...");
    delay(1000);
    while (digitalReadFast(SLS1) == HIGH) {
        odrv.SetVelocity(axis, 100);
        odrv_serial << "r axis" << axis << ".encoder.pos_estimate" << endl;
        enc_max = odrv.readFloat();
    }
    odrv.SetVelocity(axis, 0);
    Serial << "outer limit set to " << enc_max << endl;

    // find inner limit
    Serial.println("finding inner limit...");
    delay(1000);
    while (digitalReadFast(SLS2) == HIGH) {
        odrv.SetVelocity(axis, -100);
        odrv_serial << "r axis" << axis << ".encoder.pos_estimate" << endl;
        enc_min = odrv.readFloat();
    }
    odrv.SetVelocity(axis, 0);
    Serial << "inner limit set to " << enc_min << endl;

    delay(1000);

    return(1);
}

void switch_direction() {
    unsigned long this_trigger = millis();
    if (this_trigger - last_trigger > 1000) {
        dir = -dir;
    }
    last_trigger = this_trigger;
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
    u_k = dir*50;

    // unidirectional limit switching with hysteresis
    if ((hit_max) && (menck < enc_max - 10)) {
        hit_max = false;
        u_max = u_abs_max;
    }
    else if ((hit_min) && menck > enc_min + 10) {
        hit_min = false;
        u_min = u_abs_min;
    }
    else if ((digitalReadFast(SLS1) == LOW) || (menck >= enc_max)) { // outer limit reached
        hit_max = true;
        u_max = 0;
    }
    else if ((digitalReadFast(SLS2) == LOW) || (menck <= enc_min)) { // inner limit reached
        hit_min = true;
        u_min = 0;
    }
    u_k = constrain(u_k, u_min, u_max);

    // write control signal
    odrv.SetVelocity(axis, u_k);

    // update past values
    menck1 = menck;

    // print input/output data with timestamp
    Serial
        << t/1e6 << ','
        // << apot << ','
        << menck << ','
        << mvelk << ','
        << u_k << endl;

}

void loop() {
}
