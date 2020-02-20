/* gts_test.ino
 *
 * test script for measuring engine rpm from a MLX90217 geartooth sensor and a PCF8593 event counter
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 9 February 2020
 */

#include <Wire.h>
#include <PCF8593.h>

byte csr = 0x00;
byte alarm_ctrl = 0x00;
byte alarm_val = 0x00;

unsigned long count = 0;
unsigned long last_count = 0;
double engine_pulseToRpm = 35.714285714285715;
double rpm = 0;

void setup() {

  // control and status register (00h)
  csr |= 1<<5; // event counter mode
  csr |= 1<<2; // enable alarm control register (08h)

  // alarm control register (08h)
  alarm_ctrl |= 0<<4; // event alarm
  alarm_ctrl |= 0<<7; // alarm no-interrupt enable

  Wire.begin(); // join i2c bus
  
  Serial.begin(9600);

  // register setup
  set_csr(csr);
  reset_counters();
}

void loop() {
  count = get_count();
  rpm = (count - last_count)*engine_pulseToRpm;
  Serial.println(rpm);
  last_count = count;
  delay(20);
}
