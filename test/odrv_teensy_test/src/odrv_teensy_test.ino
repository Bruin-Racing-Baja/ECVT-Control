/* odrv_teensy_test.ino
 *
 * This script tests the basic functionality of the ODrive motor driver with the ASCII protocol on a Teensy
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 29 February 2020
 */

#include <ODriveArduino.h>
#include <signals.h>

// Printing with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

// setup serial to ODrive
#define odrv_serial Serial1
ODriveArduino odrv(odrv_serial);

const byte axis = 1;
char endl = '\n';
int ts = 10; // [ms]

int i = 0;

void setup() {

    // open serial to ODrive
    odrv_serial.begin(115200);

    // open serial to computer
    Serial.begin(115200);
    while(!Serial);
    Serial.println("----- ODrive ino test -----");

    // check bus voltage
    double vbus = 0.;
    while (vbus < 20) {
        odrv_serial << "r vbus_voltage" << endl;
        vbus = odrv.readFloat();
        Serial << "Bus voltage: " << vbus << endl;
    }

    // sinusoidal velocity test
    Serial << "----- Sine test -----" << endl;
    odrv.run_state(axis, ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL, false);
    Serial << "Motor armed" << endl;
    delay(1000);
    int sine_per = 5000; // [ms]
    unsigned long t0 = millis();
    unsigned long t_last = t0;
    unsigned long t = t0;
    while (t - t0 < 2*sine_per) {
        t = millis();
        int vel_ref = 200*sin(2*PI/sine_per*t) + 400;
        odrv.SetVelocity(axis, vel_ref);
        t_last = t;
        odrv_serial << "r axis" << axis << ".encoder.pos_estimate" << endl;
        double pos = odrv.readFloat();
        Serial << t << "," << vel_ref << "," << pos << endl;
        delay(max(0, ts - (millis() - t_last)));
    }
    delay(100);
    odrv.SetVelocity(axis, 0);
    odrv.run_state(axis, ODriveArduino::AXIS_STATE_IDLE, false);

    // chirp velocity test
    Serial << "----- Chirp test -----" << endl;
    int chirp_amp = 200;
    double chirp_w1 = 2*PI*.1;
    double chirp_w2 = 2*PI*10;
    double chirp_per = 10;
    double chirp_off = 400;
    configure_chirp(chirp_amp, chirp_w1, chirp_w2, chirp_per, chirp_off);
    int N = chirp_per*1000/ts;
    t0 = millis();
    for (int i = 0; i < N; i++) {
        unsigned long t = millis();
        Serial << i << ',' << chirp(t - t0) << endl;
        delay(max(0, ts - (millis() - t)));
    }
    delay(1000);
    Serial << "Test complete." << endl;
}

void loop() {
    // odrv_serial << "r axis" << axis << ".encoder.pos_estimate\n";
    // double pos = odrv.readFloat();
    // Serial << pos << endl;
}