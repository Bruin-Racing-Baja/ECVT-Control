#include <TimerOne.h>

const byte gts_pin = 2;
unsigned long counts(0);
unsigned long counts_last(0);
long rpm(0);
int dt = 10e3; // [us]

void setup() {
    Serial.begin(256000);

    pinMode(gts_pin, INPUT);

    interrupts();
    attachInterrupt(gts_pin, count_pulse, FALLING);
    Timer1.initialize(dt);
    Timer1.attachInterrupt(calc_rpm);
}

void loop() {
    Serial.println(rpm);
    delay(10);
}

void count_pulse() {
    counts += 1;
}

void calc_rpm() {
    noInterrupts();
    rpm = (counts - counts_last)*1000000/dt/80*60;
    counts_last = counts;
    interrupts();
}
