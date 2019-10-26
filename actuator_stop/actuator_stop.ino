/* actuator_stop.ino
 * this sketch writes a stop command to the actuator
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 3/10/19
 */

#include <Servo.h>

#define UK_STOP 1515
Servo Actuator;
const byte pin_actuator = 9;

void setup() {
  Actuator.attach(pin_actuator);
}

void loop() {
  Actuator.writeMicroseconds(UK_STOP);
  delay(20);
}
