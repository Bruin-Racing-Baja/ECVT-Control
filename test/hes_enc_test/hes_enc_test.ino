/* hes_enc_test.ino
 *
 * calculates engine and wheel RPM by treating hall effect sensors as unsigned
 * incremental encoders
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 */

// timing
unsigned long last_print_millis(0); // [ms]
const byte print_period = 20; // [ms]

// engine rpm
const byte eg_hall_pin = A3;
bool eg_state(LOW);
unsigned long eg_count = 0;
#define HF_HIGH 800
#define HF_LOW 200

// gearbox rpm
const byte gb_hall_pin = 3;
bool gb_state(LOW);
unsigned long gb_count = 0;

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // setup engine sensor
  pinMode(eg_hall_pin, INPUT);

  // setup gearbox sensor
  pinMode(gb_hall_pin, INPUT);

}

void loop() {

  // check time
  unsigned long current_millis = millis();

  // check sensors
  check_eg_sensor();
  check_gb_sensor();

  // print
  if (current_millis - last_print_millis >= print_period) {
    Serial.print(current_millis);
    Serial.print(",");
    Serial.print(eg_count);
    Serial.print(",");
    Serial.println(gb_count);
    last_print_millis = current_millis;
  }

}

void check_eg_sensor() {

  unsigned int eg_reading = analogRead(eg_hall_pin);
  if (eg_reading > HF_HIGH) {
    eg_state = HIGH;
  } else if (eg_reading < HF_LOW && eg_state) {
    eg_count += 1;
    eg_state = LOW;
  }

}

void check_gb_sensor() {

  bool gb_reading = digitalRead(gb_hall_pin);
  if (gb_reading) {
    gb_state = HIGH;
  } else if (gb_state) {
    gb_count += 1;
    gb_state = LOW;
  }

}
