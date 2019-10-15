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
#define GB_TORQUE 128 // ~ 8 mph
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
const int control_period = 20000; // [us]
const double Ts = control_period/1000000.0; // control_period [s]
const double Kp = 1;
unsigned long last_control_time(0);

// engine sensor
#define HF_HIGH 800
#define HF_LOW 200
const byte engine_pin = A3;
bool engine_state = LOW;
unsigned long engine_trigger_time(0);
unsigned long engine_last_trigger(0);
unsigned int engine_rpm(0);

// gearbox sensor
const byte gb_pin = 3;
bool gb_state = HIGH;
unsigned long gb_trigger_time(0);
unsigned long gb_last_trigger(0);
unsigned int gb_rpm(0);

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
//  Serial.begin(9600);

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  current_pos = analogRead(pot_pin);

  // setup engine sensor
  pinMode(engine_pin, INPUT);
  init_readings(engine_readings);

  // setup gearbox sensor
  pinMode(gb_pin, INPUT);
  init_readings(gearbox_readings);
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
  if (gearbox_rpm_ave > GB_POWER) {
    r_k = EG_POWER;
  } else if (gearbox_rpm_ave > GB_TORQUE) {
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
  if (gearbox_rpm_ave < GB_LAUNCH) {
    // u_k_max = 0;
    if (engine_rpm_ave >= EG_TORQUE) {
      u_k_min = U_K_LAUNCH_FAST;
    } else if (engine_rpm_ave >= EG_ENGAGE) {
      u_k_min = U_K_LAUNCH_SLOW;
    }
  }
  // ***** POT LIMITS ***** //
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

  // check engine_rpm
  int engine_reading = analogRead(engine_pin);
  if (engine_reading > HF_HIGH) {
    engine_state = HIGH;
  }
  if (engine_reading < HF_LOW && engine_state == HIGH) {
    engine_trigger_time = current_micros;
    engine_rpm = 60000000.0 / (engine_trigger_time - engine_last_trigger);
    engine_readings[engine_index] = engine_rpm;
    engine_index = (engine_index + 1) % num_readings;
    engine_last_trigger = engine_trigger_time;
    engine_state = LOW;
  } else if (current_micros - engine_last_trigger >= 1000000) {
    init_readings(engine_readings);
    engine_last_trigger = current_micros;
  }

  // check gearbox rpm
  int gb_reading = digitalRead(gb_pin);
  if (digitalRead(gb_pin) == LOW) {
    gb_state = LOW;
  }
  if (gb_reading == HIGH && gb_state == LOW) {
    gb_trigger_time = current_micros;
    gb_rpm = (11020408.0)/(gb_trigger_time - gb_last_trigger);
    gearbox_readings[gearbox_index] = gb_rpm;
    gearbox_index = (gearbox_index + 1) % num_readings;
    gb_last_trigger = gb_trigger_time;
    gb_state = HIGH;
  }

  // control loop
  if (current_micros - last_control_time >= control_period) {
    current_pos = analogRead(pot_pin);
    control_function();
    last_control_time = current_micros;

//    Serial.print(r_k);
//    Serial.print(" ");
//    Serial.print(gearbox_rpm_ave);
//    Serial.print(" ");
//    Serial.print(engine_rpm_ave);
//    Serial.print(" ");
//   Serial.print(current_pos);
//   Serial.print("\n");
  }
}
