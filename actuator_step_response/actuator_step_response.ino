#include <Servo.h>

#define UK_STOP 1460
#define UK_MAX 2000
#define UK_MIN 1000

const int pin_pot = A0;
const int pin_actuator = 9;

Servo Actuator;

void setup() {
  pinMode(pin_pot, INPUT);
  Actuator.attach(pin_actuator, UK_MIN, UK_MAX);
  Actuator.writeMicroseconds(UK_STOP);
  Serial.begin(9600);
  Serial.println(analogRead(pin_pot));
  delay(5e3);
}

void loop() {
  Actuator.writeMicroseconds(UK_MIN);
  Serial.println(analogRead(pin_pot));
  delay(20);
}
