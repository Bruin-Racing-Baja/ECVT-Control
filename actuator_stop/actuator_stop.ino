/* actuator_stop.ino
 * this sketch writes a stop command to the actuator
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 3/10/19
 */

#include <Servo.h>

#define UK_STOP 1515
#define UK_MAX 2000
#define UK_MIN 1000

const int pin_actuator = 9;

Servo Actuator;

void setup() {
  Actuator.attach(pin_actuator, UK_MIN, UK_MAX);
  Serial.begin(9600);
}

void loop() {
  Actuator.writeMicroseconds(UK_STOP);
  Serial.println(analogRead(A0));
  delay(20);
}
