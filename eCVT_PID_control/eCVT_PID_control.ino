/* ecvt_PID_control.ino
 * this sketch runs a PID controller on the ecvt
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 4/27/19 
 */

#include <Servo.h>

// PWM constants
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
#define u_k_limit 70
const int u_k_min = PW_MIN - PW_STOP;
const int u_k_max = PW_MAX - PW_STOP;

// reference signals
#define MAX_TORQUE 2750
#define MAX_POWER 3500
#define LAUNCH 2200
#define THRESH_LOW 75 // ~5 mph
#define THRESH_HIGH 365 // ~25 mph

// actuator
Servo Actuator;
const byte actuator_pin = 9;
#define POT_MARGIN 0
int POT_MIN = 167 + POT_MARGIN;
int POT_MAX = 294 - POT_MARGIN;
int POT_ENGAGE = 245;
const byte pot_pin = A0;
int current_pos(0);

// "14th birthday" controller
int r_k = MAX_TORQUE;
int e_k(0);
int e_k1(0);
int e_k_sum(0);
int u_k(0);
int u_k1(0);
const byte controlPeriod = 20; // [ms]
const double Ts = controlPeriod/1000.0; // controlPeriod [s]
const double Kp = 10;
const double Ki = 2.744;
const double Kd = 1.252;
const int N = 100;
unsigned long lastControlTime(0);

// engine sensor
#define HF_HIGH 800
#define HF_LOW 200
bool im_high = false;
const byte engine_pin = A3;
unsigned long engine_trigger_time(0);
unsigned long engine_last_trigger(0);
unsigned int engine_rpm(0);  

// gearbox sensor
const byte gearbox_pin = 3;
unsigned long gearbox_trigger_time(0);
unsigned long gearbox_last_trigger(0);
unsigned int gearbox_rpm(0);

// brake sensor
const byte brake_pin = 8;
const int u_k_brake = 311;
bool brake(false);

// moving average filters
const size_t num_readings = 4;
unsigned int engine_rpm_ave(0);
byte engine_index = 0;
unsigned int engine_readings[num_readings];
unsigned int gearbox_rpm_ave(0);
byte gearbox_index = 0;
unsigned int gearbox_readings[num_readings];

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // open serial connection
  Serial.begin(9600);

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  current_pos = analogRead(pot_pin);

  // setup engine sensor
  pinMode(engine_pin, INPUT);
  init_readings(engine_readings);

  // setup gearbox sensor
  pinMode(gearbox_pin, INPUT);
  init_readings(gearbox_readings);
  attachInterrupt(digitalPinToInterrupt(gearbox_pin), gearbox_isr, CHANGE);
  interrupts();

  // create timer interrupt
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) {
  int current_millis = millis();
  if (current_millis - lastControlTime >= controlPeriod) {
    control_function();
    lastControlTime = current_millis;

    // Serial.print(r_k);
    // Serial.print(" ");
    // Serial.print(u_k);
    // Serial.print(" ");
    // Serial.print(engine_rpm);
    // Serial.print(" ");
    Serial.print(engine_rpm_ave);
    // Serial.print(" ");
    // Serial.print(gearbox_rpm);
    Serial.print(" ");
    Serial.print(gearbox_rpm_ave);
    // Serial.print(" ");
    // Serial.print(current_pos);
    // Serial.print(" ");
    // Serial.print(brake);
    // Serial.print(" ");
    // Serial.print(millis());
    Serial.print("\n");
  }
}

void gearbox_isr() {
  gearbox_trigger_time = millis();
  // rpm to mph .068425
  gearbox_rpm = 5510.204 / (gearbox_trigger_time - gearbox_last_trigger);
  gearbox_readings[gearbox_index] = gearbox_rpm;
  gearbox_index = (gearbox_index + 1) % num_readings;
  gearbox_last_trigger = gearbox_trigger_time;
}

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

void control_function() {

  // calculate gearboxrpm
  gearbox_rpm_ave = rpm_average(gearbox_readings);

  // adjust reference
  if (gearbox_rpm_ave < THRESH_LOW) {
    r_k = LAUNCH;
  } else if (gearbox_rpm_ave > THRESH_HIGH) {
    r_k = MAX_POWER;
  } else {
    r_k = MAX_TORQUE;
  }
  
  // calculate engine rpm
  engine_rpm_ave = rpm_average(engine_readings);

  // compute error
  e_k = r_k - engine_rpm_ave;
  e_k_sum = constrain(e_k_sum + e_k, u_k_min, u_k_max);

  // check brake
  brake = digitalRead(brake_pin);

  // compute control signal
  u_k = Kp*e_k + Ki*Ts/2*e_k_sum + u_k1 + Kd*N*(e_k-e_k1)-(N*Ts-1)*u_k1 + u_k_brake*brake;
  u_k = constrain(u_k, u_k_min, u_k_max);

  // force shift out if rpm is too low
  if (engine_rpm_ave < 1000) {
    u_k = u_k_max;
  }

  // software limit switches
  if (current_pos >= POT_ENGAGE) {
    u_k = -u_k_limit;
  } else if (current_pos <= POT_MIN) {
    u_k = u_k_limit;
  }

  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);

  // update past values
  e_k1 = e_k;
  u_k1 = u_k;
}

void loop() {

  // update pot position
  current_pos = analogRead(pot_pin);

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
}
