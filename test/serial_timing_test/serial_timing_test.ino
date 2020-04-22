/* serial_timing_test.ino
 *  
 *  this script is meant to test the timing differences between Serial.print and Serial.write
 */
int val;
int inc = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(256000);

//  delay(1000);
//  const int n = 1e2;
//  
//  int bytes_print = 0;
//  unsigned long start_print_t = millis();
//  for (int i = 0; i < n; i++) {
//    // Serial.println("Peppa for the last time get out of my asshole");
////    bytes_print += Serial.println(200);
//  }
//  unsigned long end_print_t = millis();
//
//  delay(1000);
//
//  unsigned long start_write_t = millis();
//  int bytes_write = 0;
//  for (int i = 0; i < n; i++) {
//    // Serial.write("Peppa for the last time get out of my asshole");
//    bytes_write += Serial.write(analogRead(A0));
//    Serial.write("\n");
//  }
//  unsigned long end_write_t = millis();
//
//  Serial.print("\nSerial.print: ");
//  Serial.print(end_print_t - start_print_t);
//  Serial.println(" ms");
//  Serial.print("Serial.write: ");
//  Serial.print(end_write_t - start_write_t);
//  Serial.println(" ms");
//  Serial.println(bytes_print);
//  Serial.println(bytes_write);

//  int bytes_written = 0;
//  byte val = -129;
//  for (int i = 0; i < 10; i++) {
//    bytes_written += Serial.write(val);
//  }
//  Serial.println(bytes_written);
}
 
void loop() {
  val += inc;
  if (abs(val) >= 127) {
    inc *= -1;
  }
  delay(10);
  Serial.write(val);
  Serial.write(-val); 
  
}
