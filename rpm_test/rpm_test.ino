#define NUM_MAGNETS 1
const int sensor_pin = 7;
int rpm(0);
int last_trigger(0);
int trigger_time(0);
int HighLow = LOW;
bool RPMCount = false;


void setup() {
  pinMode(sensor_pin, INPUT);
  Serial.begin(9600);
  last_trigger = millis();
}

void loop(){
  RPMCalc();
  Serial.println(rpm);
}

void RPMCalc() {
  if (digitalRead(sensor_pin) == LOW) {
    if (HighLow == 1) {
      RPMCount = 1;
    }
    HighLow = 0;
  }
  if (digitalRead(sensor_pin) == HIGH) {
    HighLow = 1;
  }
  if (RPMCount == 1) {
    trigger_time = micros();
    delta_t = trigger_time - last_trigger;
    rpm = 60000000.0 / delta_t;
    RPMCount = 0;
    last_trigger = trigger_time;
  }
}

//void loop() {
////  Serial.println(digitalRead(sensor_pin));
//  Serial.println(rpm);
//}
//
//void calculate_rpm() {
//  //detachInterrupt(digitalPinToInterrupt(sensor_pin));
//  int trigger_time = millis();
//  rpm = 60000/NUM_MAGNETS/(trigger_time - last_trigger);
//  last_trigger = trigger_time;
//  //attachInterrupt(digitalPinToInterrupt(sensor_pin), calculate_rpm, FALLING);
//}

