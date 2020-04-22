/* hes_interrupt_test.ino
 *
 * tests engine RPM calculation based on an interrupt activated once per engine
 * revolution due to an analog hall sensor
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 */

const byte eg_pin = 2;
volatile double eg_rpm(0);
volatile unsigned long previous_millis(0);

void setup() {
  // put your setup code here, to run once:
  pinMode(eg_pin, INPUT);
  previous_millis = micros();
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), eg_interrupt, RISING);
}

void eg_interrupt() {
  unsigned long current_millis = micros();
  eg_rpm = 60.0e6/(current_millis - previous_millis);
  previous_millis = current_millis;
}

void loop() {
  Serial.println(eg_rpm);
//  Serial.println(digitalRead(2));
}
