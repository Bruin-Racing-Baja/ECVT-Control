#define NUM_MAGNETS 1
const byte sensor_pin = A5;
long rpm(0);
long last_trigger(0);
long trigger_time(0);
int HighLow = LOW;
bool RPMCount = false;
int index = 0;
const int num_readings = 4;
long readings[num_readings];
int dt(0);


void init_readings() {
  for (int i = 0; i < num_readings; i++) {
    readings[i] = 0;
  }
}

unsigned long rpm_average() {
//  detachInterrupt(digitalPinToInterrupt(sensor_pin));
  unsigned int sum = 0;
  for (int i = 0; i < num_readings; i++) {
    sum += readings[i];
  }
//  attachInterrupt(digitalPinToInterrupt(sensor_pin), RPMIsr, FALLING);
  return (sum / num_readings);
}

void setup() {
  init_readings();
  pinMode(sensor_pin, INPUT);
  Serial.begin(9600);
  last_trigger = millis();

//  attachInterrupt(digitalPinToInterrupt(sensor_pin), RPMIsr, FALLING);
}

void loop() {
  RPMCalc();
  Serial.print(rpm);
  Serial.print(" ");
//  Serial.print(rpm_average());
//  Serial.print(" ");
//  Serial.print(dt);
  Serial.print("\n");
}

void RPMCalc() {
   if (analogRead(sensor_pin) <= 100) {
     if (HighLow == 1) {
       trigger_time = millis();
//       Serial.println(trigger_time);
       rpm = 60000.0 / (trigger_time - last_trigger);
//       readings[index] = rpm;
//       index = (index + 1) % num_readings;
       last_trigger = trigger_time;
     }
     HighLow = 0;
   }
   if (analogRead(sensor_pin) >= 800) {
     HighLow = 1;
   }
}

void RPMIsr() {
  trigger_time = millis();
  dt = trigger_time - last_trigger;
  rpm = 60000.0 / (dt);
//      readings[index] = rpm;
//      index = (index + 1) % num_readings;
//  Serial.println(dt);
  last_trigger = trigger_time;
}
