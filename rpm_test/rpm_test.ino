#define NUM_MAGNETS 1
const byte sensor_pin = 6;
unsigned int rpm(0);
unsigned long last_trigger(0);
unsigned long trigger_time(0);
int HighLow = LOW;
bool RPMCount = false;
int index(0);
const int num_readings = 5;
unsigned int readings[num_readings];


void init_readings() {
  for (int i = 0; i < num_readings; i++) {
    readings[i] = 0;
  }
}

unsigned int rpm_average() {
  unsigned int sum = 0;
  for (int i = 0; i < num_readings; i++) {
    sum += readings[i];
  }
  return (sum / num_readings);
}

void setup() {
  init_readings();
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
      trigger_time = millis();
      rpm = 60000.0 / (trigger_time - last_trigger);
      readings[index] = rpm;
      index = (index + 1) % num_readings;
      last_trigger = trigger_time;
    }
    HighLow = 0;
  }
  if (digitalRead(sensor_pin) == HIGH) {
    HighLow = 1;
  }
}
