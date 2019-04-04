#include <Servo.h>

#define UK_STOP 1460
#define UK_MAX 2000
#define UK_MIN 1000

int uk = UK_STOP;

const int pin_pot = A0;
const int pin_actuator = 9;

Servo Actuator;

void write_command() {
  uk = UK_MIN;
}

void setup() {
  pinMode(pin_pot, INPUT);
  Actuator.attach(pin_actuator, UK_MIN, UK_MAX);
  Actuator.writeMicroseconds(UK_STOP);
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(3), write_command, RISING);
}

void loop() {
  Actuator.writeMicroseconds(uk);
  Serial.print(uk);
  Serial.print(" ");
  Serial.println(analogRead(pin_pot));
  delay(20);
}
