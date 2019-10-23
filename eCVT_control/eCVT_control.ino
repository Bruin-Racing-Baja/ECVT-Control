#include <Servo.h>

// PWM/control signal constants
#define PW_STOP 1551 // [pwm]
#define PW_MIN 1000 // [pwm]
#define PW_MAX 2000 // [pwm]
const int U_MIN = PW_MIN - PW_STOP; // [dpwm]
const int U_MAX = PW_MAX - PW_STOP; // [dpwm]

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
unsigned long current_millis(0); // [ms]

// scaling constants
#define RPM_PREDIV 10
#define FILT_GAIN 1e2

// reference signals [rpm/10]
// ***** ENGINE ***** //
const unsigned int EG_IDLE = 1750 * FILT_GAIN / RPM_PREDIV;
const unsigned int EG_ENGAGE = 2100 * FILT_GAIN / RPM_PREDIV;
const unsigned int EG_LAUNCH = 2600 * FILT_GAIN / RPM_PREDIV;
const unsigned int EG_TORQUE = 2700 * FILT_GAIN / RPM_PREDIV;
const unsigned int EG_POWER = 3400 * FILT_GAIN / RPM_PREDIV;
// ***** GB ***** //
const unsigned int GB_LAUNCH = 80 * FILT_GAIN / RPM_PREDIV; // ~ 5 mph
const unsigned int GB_TORQUE = 128 * FILT_GAIN / RPM_PREDIV; // ~ 8 mph
const unsigned int GB_POWER = 621.6 * FILT_GAIN / RPM_PREDIV; // ~ 39 mph

// controller
unsigned long last_control_millis(0); // [ms]
const byte control_period = 20; // [ms]
unsigned int r_k = EG_TORQUE; // [rpm/10]
const byte Kp = 1;

// engine rpm
const byte eg_hall_pin = A3;
bool eg_state(LOW);
unsigned long eg_count = 0;
unsigned long eg_count_prev = 0;
#define HF_HIGH 800
#define HF_LOW 200
// conversion from ticks/sample to rpm/RPM_PREDIV:
//  1/control_period samples/us
//  1e3 ms/s
//  1 rev/tick
//  60 s/min
const unsigned int eg_count2rpm = 1e3*60/control_period/RPM_PREDIV;
const byte eg_filt_const_1 = 88;
const byte eg_filt_const_2 = -77;
unsigned int eg_rpm_raw(0); // [rpm/10]
unsigned int eg_rpm(0); // [rpm*10]

// gearbox rpm
const byte gb_hall_pin = 3;
bool gb_state(LOW);
unsigned long gb_count = 0;
unsigned long gb_count_prev = 0;
// conversion from ticks/sample to rpm/RPM_PREDIV:
//  1/control_period samples/us
//  1e3 ms/s
//  1/4 rev/tick
//  60 s/min
//  49/18 gear ratio
const unsigned int gb_count2rpm = 1e3*60*49/control_period/4/18/RPM_PREDIV;
const int gb_filt_const_1 = 56;
const int gb_filt_const_2 = -11;
unsigned int gb_rpm_raw(0); // [rpm/10]
unsigned int gb_rpm(0); // [rpm*10]

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  Serial.begin(9600);

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
  current_millis = millis();

  // check time on control loop
  if (current_millis - last_control_millis >= control_period) {
    control_loop();
    last_control_millis = current_millis;
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

  // that's all for now, print and return
  Serial.println(eg_count);
  return;

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
  int e_k = (int) r_k - eg_rpm; // [rpm*10]
  
  // calculate control signal
  int u_k = Kp*e_k; // [dpwm*10]
  u_k = u_k * RPM_PREDIV / FILT_GAIN; // [dpwm]
  u_k = constrain(u_k, u_min, u_max);
  
  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);

  last_control_millis = current_millis;
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
  eg_rpm_raw = (eg_count - eg_count_prev)*eg_count2rpm;

  // calculate filtered rpm
  eg_rpm = eg_filt_const_1*(eg_rpm_raw + eg_rpm_raw_prev) + eg_filt_const_2*eg_rpm/FILT_GAIN;

  // update previous values
  eg_count_prev = eg_count;
  
}

void calc_gb_rpm() {
  
  // calculate raw rpm
  unsigned int gb_rpm_raw_prev = gb_rpm_raw;
  gb_rpm_raw = (gb_count - gb_count_prev)*gb_count2rpm;

  // calculate filtered rpm
  gb_rpm = gb_filt_const_1*(gb_rpm_raw + gb_rpm_raw_prev) + gb_filt_const_2*gb_rpm/FILT_GAIN;

  // update previous values
  gb_count_prev = gb_count;
  
}
