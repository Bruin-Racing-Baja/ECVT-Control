#include <Servo.h>

// PWM/control signal constants
#define PW_STOP 1551 // [pwm]
#define PW_MIN 1000 // [pwm]
#define PW_MAX 2000 // [pwm]
#define U_MIN PW_MIN - PW_STOP // [dpwm]
#define U_MAX PW_MAX - PW_STOP // [dpwm]

// actuator constants
Servo Actuator;
const byte actuator_pin = 9;
const byte pot_pin = A1;
#define POT_MIN 163
#define POT_MAX 254
#define POT_ENGAGE 245

// actuator commands
unsigned int pot_lim_out = POT_MAX;
unsigned int pot_lim_in = POT_MIN;
int u_min = U_MIN; // [dpwm]
int u_max = U_MAX; // [dpwm]
unsigned int actuator_pos(0);

// timing
unsigned long current_micros(0); // [us]

// filter gain
#define FILT_GAIN 1e3

// reference signals
// ***** ENGINE ***** //
#define EG_IDLE 1750 * FILT_GAIN
#define EG_ENGAGE 2100 * FILT_GAIN
#define EG_LAUNCH 2600 * FILT_GAIN
#define EG_TORQUE 2700 * FILT_GAIN
#define EG_POWER 3400 * FILT_GAIN
// ***** GB ***** //
#define GB_LAUNCH 80 * FILT_GAIN // ~ 5 mph
#define GB_TORQUE 128 * FILT_GAIN // ~ 8 mph
#define GB_POWER 621.6 * FILT_GAIN // ~ 39 mph

// controller
#define CONTROLLER_GAIN 10
unsigned long last_control_micros(0); // [us]
const unsigned int control_period = 20e3; // [us]
int r_k = EG_TORQUE;
const byte Kp = 1;

// engine rpm
const byte eg_hall_pin = A3;
bool eg_state(LOW);
unsigned long eg_count = 0;
unsigned long eg_count_prev = 0;
#define HF_HIGH 800
#define HF_LOW 200
// conversion from ticks/sample to rpm:
//  1/control_period samples/us
//  1e6 us/s
//  1 rev/tick
//  60 s/min
const int eg_count2rpm_num = 1e6*60; // TODO
const int eg_count2rpm_denom = control_period; // TODO
const int eg_filt_const_1 = 883;
const int eg_filt_const_2 = -766;
unsigned int eg_rpm_raw(0);
unsigned int eg_rpm(0);

// gearbox rpm
const byte gb_hall_pin = 3;
bool gb_state(LOW);
unsigned long gb_count = 0;
unsigned long gb_count_prev = 0;
// conversion from ticks/sample to rpm:
//  1/control_period samples/us
//  1e6 us/s
//  1/4 rev/tick
//  60 s/min
//  49/18 gear ratio
const int gb_count2rpm_num = 1e6*60*49; // TODO
const int gb_count2rpm_denom = control_period*4*18; // TODO
const int gb_filt_const_1 = 557;
const int gb_filt_const_2 = -114;
unsigned int gb_rpm_raw(0);
unsigned int gb_rpm(0);

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  actuator_pos = analogRead(pot_pin);

  // setup engine sensor
  pinMode(eg_hall_pin, INPUT);

  // setup gearbox sensor
  pinMode(gb_hall_pin, INPUT);
}

void loop() {
  
  // check time
  current_micros = micros();

  // check time on control loop
  if (current_micros - last_control_micros >= control_period) {
    control_loop();
    last_control_micros = current_micros;
  }

  // check sensors
  actuator_pos = analogRead(pot_pin);
  check_eg_sensor();
  check_gb_sensor();
  
}

void control_loop() {

  // calculate rpms
  calc_eg_rpm();
  calc_gb_rpm();

  // adjust reference
  if (gb_rpm > GB_POWER) {
    r_k = EG_POWER;
  } else if (gb_rpm > GB_TORQUE) {
    r_k = map(gb_rpm, GB_LAUNCH, GB_POWER, EG_LAUNCH, EG_POWER);
  } else {
    r_k = EG_LAUNCH;
  }

  // change pot outer limit
  if (eg_rpm < EG_ENGAGE) {
    pot_lim_out = map(eg_rpm, EG_IDLE, EG_ENGAGE, POT_MAX, POT_ENGAGE);
  } else {
    pot_lim_out = POT_ENGAGE;
  }

  // enforce pot limits
  u_max = U_MAX;
  u_min = U_MIN;
  if (actuator_pos >= pot_lim_out) {
    u_max = 0;
  } else if (actuator_pos <= pot_lim_in) {
    u_min = 0;
  }

  // calculate error
  int e_k = r_k - eg_rpm;
  
  // calculate control signal
  int u_k = Kp*e_k;
  u_k /= FILT_GAIN*CONTROLLER_GAIN;
  u_k = constrain(u_k, u_min, u_max);
  
  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);

  last_control_micros = current_micros;
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

void calc_eg_rpm() {

  // calculate raw rpm
  unsigned int eg_rpm_raw_prev = eg_rpm_raw;
  eg_rpm_raw = (eg_count - eg_count_prev)*eg_count2rpm_num/eg_count2rpm_denom;

  // calculate filtered rpm
  eg_rpm = eg_filt_const_1*(eg_rpm_raw + eg_rpm_raw_prev) + eg_filt_const_2*eg_rpm/FILT_GAIN;

  // update previous values
  eg_count_prev = eg_count;
  
}

void calc_gb_rpm() {
  
  // calculate raw rpm
  unsigned int gb_rpm_raw_prev = gb_rpm_raw;
  gb_rpm_raw = (gb_count - gb_count_prev)*gb_count2rpm_num/gb_count2rpm_denom;

  // calculate filtered rpm
  gb_rpm = gb_filt_const_1*(gb_rpm_raw + gb_rpm_raw_prev) + gb_filt_const_2*gb_rpm/FILT_GAIN;

  // update previous values
  gb_count_prev = gb_count;
  
}
