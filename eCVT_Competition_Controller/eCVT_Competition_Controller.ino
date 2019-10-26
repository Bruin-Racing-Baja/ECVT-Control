/* eCVT_Competition_Controller.ino
 * version of the competition from Baja SAE California 2019, with a few tweaks made for readability and efficiency
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu), Iou-Sheng Chang
 * created: 15 May 2019
 */

#include <Servo.h>

// PWM constants
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
#define U_K_LAUNCH_SLOW -200
#define U_K_LAUNCH_FAST -350
#define U_K_ABS_MIN PW_MIN - PW_STOP
#define U_K_ABS_MAX PW_MAX - PW_STOP

// actuator
Servo Actuator;
const byte actuator_pin = 9;
#define POT_MIN 163
#define POT_MAX 254
#define POT_ENGAGE 245
int pot_lim_out = POT_MAX;
int pot_lim_in = POT_MIN;
int u_k_min = U_K_ABS_MIN;
int u_k_max = U_K_ABS_MAX;
const byte pot_pin = A1;
int current_pos(0);

// reference signals
// ***** ENGINE ***** //
#define EG_IDLE 1750
#define EG_ENGAGE 2100
#define EG_LAUNCH 2600
#define EG_TORQUE 2700
#define EG_POWER 3400
// ***** GB ***** //
#define GB_LAUNCH 80  // ~ 5 mph
#define GB_POWER 621.6  // ~ 39 mph
// ***** ON JACK STAND ***** //
//#define RPM_IDLE 67
//#define RPM_2400 93
//#define RPM_FULLTHROTTLE 530
//#define RPM_2800 135
//#define RPM_3200 350

// controller
int r_k = EG_TORQUE;
int e_k(0);
int u_k(0);
const int control_period = 20e3; // [us]
const int Kp = 1;
unsigned long last_control_time(0);

// engine sensor
#define HF_HIGH 800
#define HF_LOW 200
#define HALL_WATCHDOG 1e6 // [us]
const byte eg_pin = A3;
bool eg_state = LOW;
unsigned long eg_last_trigger(0);
unsigned int eg_rpm_raw(0);

// gearbox sensor
const byte gb_pin = 3;
bool gb_state = HIGH;
unsigned long gb_last_trigger(0);
unsigned int gb_rpm_raw(0);

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // open serial connection
  // Serial.begin(9600);

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  current_pos = analogRead(pot_pin);

  // setup engine sensor
  pinMode(eg_pin, INPUT);
  init_readings(engine_readings);

  // setup gearbox sensor
  pinMode(gb_pin, INPUT);
  init_readings(gearbox_readings);
}

void control_function() {

  // calculate gearboxrpm
  gearbox_rpm_ave = rpm_average(gearbox_readings);

  // adjust reference
  if (gearbox_rpm_ave > GB_POWER) {
    r_k = EG_POWER;
  } else if (gearbox_rpm_ave > GB_LAUNCH) {
    r_k = map(gearbox_rpm_ave, GB_LAUNCH, GB_POWER, EG_LAUNCH, EG_POWER);
  } else {
    r_k = EG_LAUNCH;
  }

  // calculate engine rpm
  engine_rpm_ave = rpm_average(engine_readings);

  // compute error
  e_k = r_k - engine_rpm_ave;

  // compute control signal
  u_k = Kp*e_k;

  // change pot outer limit
  if (engine_rpm_ave <= EG_ENGAGE) {
    pot_lim_out = map(engine_rpm_ave, EG_IDLE, EG_ENGAGE, POT_MAX, POT_ENGAGE);
  } else {
    pot_lim_out = POT_ENGAGE;
  }

  // constrain control output
  // ***** PWM LIMITS ***** //
  u_k_min = U_K_ABS_MIN;
  u_k_max = U_K_ABS_MAX;
  // ***** LAUNCH ***** //
  if (gearbox_rpm_ave < GB_LAUNCH && engine_rpm_ave >= EG_TORQUE) {
    u_k_min = U_K_LAUNCH_FAST;
  }
  // ***** POT LIMITS ***** //
  current_pos = analogRead(pot_pin);
  if (current_pos >= pot_lim_out) {
    u_k_max = 0;
  } else if (current_pos <= pot_lim_in) {
    u_k_min = 0;
  }
  u_k = constrain(u_k, u_k_min, u_k_max);

  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);
}

void loop() {

  // check time
  unsigned long current_micros = micros();

  // check engine rpm
  int eg_reading = analogRead(eg_pin);
  if (eg_reading > HF_HIGH) {
    eg_state = HIGH;
  } else if (eg_reading < HF_LOW && eg_state == HIGH) {
    eg_rpm_raw = 60000000.0/(current_micros - eg_last_trigger);
    eg_last_trigger = current_micros;
    eg_state = LOW;
  }
  if (current_micros - eg_last_trigger >= HALL_WATCHDOG) {
    eg_rpm_raw = 0;
    eg_last_trigger = current_micros;
  }

  // check gearbox rpm
  bool gb_reading = digitalRead(gb_pin);
  if (gb_reading == HIGH) {
    gb_state = HIGH;
  } else if (gb_reading == LOW && gb_state == HIGH) {
    gb_rpm_raw = (11020408.0)/(current_micros - gb_last_trigger);
    gb_last_trigger = current_micros;
    gb_state = LOW;
  }
  if (current_micros - gb_last_trigger >= HALL_WATCHDOG) {
    gb_rpm_raw = 0;
    gb_last_trigger = current_micros;
  }

  // control loop
  if (current_micros - last_control_time >= control_period) {
    control_function();
    last_control_time = current_micros;

    // Serial.print(r_k);
    // Serial.print(" ");
    // Serial.print(gearbox_rpm_ave);
    // Serial.print(" ");
    // Serial.print(engine_rpm_ave);
    // Serial.print(" ");
    // Serial.print(current_pos);
    // Serial.print("\n");
  }
}
