#define NUM_MAGNETS 1
const byte sensor_pin = 6;
unsigned int rpm(0);
unsigned long last_trigger(0);
unsigned long trigger_time(0);
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
    trigger_time = millis();
    rpm = 60000.0 / (trigger_time - last_trigger);
    RPMCount = 0;
    last_trigger = trigger_time;
  }
}
