#define NUM_MAGNETS 1
const int sensor_pin = 3;
int last_trigger(0);
int rpm(0);

void setup() {
  pinMode(sensor_pin, INPUT);
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(3), calculate_rpm, FALLING);
}

void loop() {
//  Serial.println(digitalRead(sensor_pin));
  Serial.println(rpm);
}

void calculate_rpm() {
  int trigger_time = millis();
  rpm = 60000/NUM_MAGNETS/(trigger_time - last_trigger);
  last_trigger = trigger_time;
}

