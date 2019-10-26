/* rpm_test.ino
 * this sketch runs only the engine rpm measurement portion of the control code and prints it to the console for debugging.
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 3 May, 2019
 */

#define HF_HIGH 800
#define HF_LOW 100
bool im_high = false;
const byte engine_pin = A3;
unsigned long engine_trigger_time(0);
unsigned long engine_last_trigger(0);
unsigned int engine_rpm(0); 

const size_t num_readings = 4;
unsigned int engine_rpm_ave(0);
byte engine_index = 0;
unsigned int engine_readings[num_readings];


void init_readings(unsigned int* readings) {
  for (int i = 0; i < num_readings; i++) {
    readings[i] = 0;
  }
}

unsigned int rpm_average(const unsigned int* readings) {
  unsigned int sum = 0;
  for (int i = 0; i < num_readings; i++) {
    sum += readings[i];
  }
  return (sum / num_readings);
}

void setup() {
  Serial.begin(9600);
  pinMode(engine_pin, INPUT);
  init_readings(engine_readings);
  engine_last_trigger = millis();
}

void loop() {
  // check engine_rpm
  int reading = analogRead(engine_pin);
  if (reading > HF_HIGH) {
    im_high = true;
  } 
  if (im_high && (reading < HF_LOW)) {
    engine_trigger_time = millis();
    engine_rpm = 60000.0 / (engine_trigger_time - engine_last_trigger);
    engine_readings[engine_index] = engine_rpm;
    engine_index = (engine_index + 1) % num_readings;
    engine_last_trigger = engine_trigger_time;
    im_high = false;
  }

  Serial.println(rpm_average(engine_readings) );
}
