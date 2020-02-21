/* ecvt_dev_comp_controller.ino
 *
 * Development version of the ecvt_competition_controller.
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 20 February 2020
 */

#include <PCF8593.h>
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
#define POT_MIN 166
#define POT_MAX 254
#define POT_ENGAGE 245
unsigned int pot_lim_out = POT_MAX;
unsigned int pot_lim_in = POT_MIN;
int u_k_min = U_K_ABS_MIN; // [dpwm]
int u_k_max = U_K_ABS_MAX; // [dpwm]
const byte pot_pin = A1;
unsigned int pot_pos(0);

// reference signals
// ***** ENGINE ***** //
const unsigned int EG_IDLE = 1750;
const unsigned int EG_ENGAGE = 2100;
const unsigned int EG_LAUNCH = 2600;
const unsigned int EG_TORQUE = 2700;
const unsigned int EG_POWER = 3400;
// ***** GB ***** //
const unsigned int GB_LAUNCH = 80; // ~ 5 mph
const unsigned int GB_POWER = 621.6; // ~ 39 mph

// controller
int r_k = EG_TORQUE;
const unsigned int control_period = 20e3; // [us]
const byte Kp = 1;
unsigned long last_control_time(0); // [us]

// engine sensor
byte csr = 0x00;
byte alarm_ctrl = 0x00;
byte alarm_val = 0x00;
unsigned long eg_count = 0;
unsigned long eg_last_count = 0;
double eg_pulseToRpm = 35.714285714285715;
int eg_rpm(0);

// gearbox sensor
#define HALL_WATCHDOG 1e6 // [us]
const byte gb_pin = 3;
bool gb_state(LOW);
unsigned long gb_last_trigger(0); // [us]
int gb_rpm(0);

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // open serial connection
  Serial.begin(256000);

  // join i2c bus
  Wire.begin();

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  pot_pos = analogRead(pot_pin);

  // setup engine sensor
  csr |= 1<<5; // event counter mode
  csr |= 1<<2; // enable alarm control register (08h)
  // alarm_ctrl |= 1<<4; // event alarm
  // alarm_ctrl |= 1<<7; // alarm interrupt enable
  set_csr(csr);
  reset_counters();
}

void control_function() {

  // calculate engine rpm
  eg_count = get_count();
  eg_rpm = (eg_count - eg_last_count)*eg_pulseToRpm;

    // change pot outer limit
  if (eg_rpm <= EG_ENGAGE) {
    pot_lim_out = map(eg_rpm, EG_IDLE, EG_ENGAGE, POT_MAX, POT_ENGAGE);
  } else {
    pot_lim_out = POT_ENGAGE;
  }

  // constrain control output
  // ***** PWM LIMITS ***** //
  u_k_min = U_K_ABS_MIN;
  u_k_max = U_K_ABS_MAX;
  // ***** LAUNCH ***** //
  if (gb_rpm < GB_LAUNCH && eg_rpm >= EG_TORQUE) {
    u_k_min = U_K_LAUNCH_FAST;
  }
  // ***** POT LIMITS ***** //
  pot_pos = analogRead(pot_pin);
  if (pot_pos >= pot_lim_out) {
    u_k_max = 0;
  } else if (pot_pos <= pot_lim_in) {
    u_k_min = 0;
  }

  // compute error
  int e_k = r_k - eg_rpm;

  // compute control signal
  int u_k = Kp*e_k;
  u_k = constrain(u_k, u_k_min, u_k_max);

  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);

  // update previous values
  eg_last_count = eg_count;
}

void loop() {

  // check time
  unsigned long current_micros = micros();

  // check gearbox rpm
  bool gb_reading = digitalRead(gb_pin);
  if (gb_reading == HIGH) {
    gb_state = HIGH;
  } else if (gb_reading == LOW && gb_state == HIGH) {
    gb_rpm = (11020408.)/(current_micros - gb_last_trigger);
    gb_last_trigger = current_micros;
    gb_state = LOW;
  }
  if (current_micros - gb_last_trigger >= HALL_WATCHDOG) {
    gb_rpm = 0;
    gb_last_trigger = current_micros;
  }

  // control loop
  if (current_micros - last_control_time >= control_period) {
    control_function();
    last_control_time = current_micros;

    Serial.print(r_k);
    Serial.print(",");
    Serial.print(eg_rpm);
    Serial.print(",");
    Serial.print(gb_rpm);
    Serial.print(",");
    Serial.print(pot_pos);
    Serial.print("\n");
  }
}
