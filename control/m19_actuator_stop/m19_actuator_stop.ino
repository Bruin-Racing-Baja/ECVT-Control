/* m19_actuator_stop.ino
 * 
 * writes a stop command to the Thomson actuator
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 10 March 2019
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
