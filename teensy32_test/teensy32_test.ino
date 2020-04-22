//#include <Encoder.h>

//Encoder myEnc(5, 6);
long counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(256000);

  attachInterrupt(5, count, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(counter);
}

void count() {
  counter += 1;
}
