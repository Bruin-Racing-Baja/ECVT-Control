/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
// #include <ODriveArduino.h>
#include <AudioStream.h>
unsigned int big_num = 0;
int n = 0;

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  // turn the LED on (HIGH is the voltage level)
  digitalWriteFast(LED_BUILTIN, HIGH);
  // wait for a second
  delay(500);
  // turn the LED off by making the voltage LOW
  digitalWriteFast(LED_BUILTIN, LOW);
   // wait for a second
  delay(500);
  Serial.println(big_num);
  big_num = -10 + pow(2, 32) + n;
  n += 1;
}
