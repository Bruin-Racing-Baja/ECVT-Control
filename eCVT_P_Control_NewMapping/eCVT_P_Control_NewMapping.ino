#include <Servo.h>

// PWM constants
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
#define U_K_LIMIT 80
#define U_K_LAUNCH_SLOW -200
#define U_K_LAUNCH_FAST -350
#define U_K_MIN PW_MIN - PW_STOP
#define U_K_MAX PW_MAX - PW_STOP

// actuator
Servo Actuator;
const byte actuator_pin = 9;
#define POT_MIN 165
#define POT_MAX 249
#define POT_ENGAGE 240
int pot_lim_out = POT_MAX;
int pot_lim_in = POT_MIN;
const byte pot_pin = A1;
int current_pos(0);

// reference signals
// ***** ENGINE ***** //
#define EG_LAUNCH 1800
#define EG_TORQUE 2600
#define EG_POWER 3100
#define EG_GEARLIMIT 3400
#define EG_KILL 1500
// ***** GB ***** //
#define GB_LAUNCH 80  // ~ 5 mph
#define GB_TORQUE 128 // ~ 8 mph
#define GB_POWER 656  // ~ 41 mph
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
const byte controlPeriod = 20; // [ms]
const double Ts = controlPeriod/1000.0; // controlPeriod [s]
const double Kp = 1;
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
const byte gbPin = 3;
unsigned int gbRPM(0);
unsigned long gbPrevMil(0);
bool gbstate(HIGH);

// brake sensor
const byte brake_pin = 8;
const int Kff = 1000;
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
  pinMode(gbPin, INPUT);
  init_readings(gearbox_readings);

  // create timer interrupt
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) {
  int current_millis = millis();
  if (current_millis - lastControlTime >= controlPeriod) {
    control_function();
    lastControlTime = current_millis;

     Serial.print(r_k);
     Serial.print(" ");
//     Serial.print(u_k);
//     Serial.print(" ");
////     Serial.print(engine_rpm);
////     Serial.print(" ");
     Serial.print(engine_rpm_ave);
     Serial.print(" ");
//     Serial.print(gearbox_rpm);
//     Serial.print(" ");
     Serial.print(gearbox_rpm_ave);
     Serial.print(" ");
//     Serial.print(current_pos);
//     Serial.print(" ");
//     Serial.print(brake);
//     Serial.print(" ");
//     Serial.print(millis());
    Serial.print("\n");
  }
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
    r_k = EG_GEARLIMIT;
  } else if (gearbox_rpm_ave > GB_TORQUE) {
    r_k = map(gearbox_rpm_ave, GB_TORQUE, GB_POWER, EG_TORQUE, EG_POWER);
  } else if (gearbox_rpm_ave > GB_LAUNCH) {
    r_k = map(gearbox_rpm_ave, GB_LAUNCH, GB_TORQUE, EG_LAUNCH, EG_TORQUE);
  } else {
    r_k = EG_LAUNCH;
  }
 
  // calculate engine rpm
  engine_rpm_ave = rpm_average(engine_readings);

  // compute error
  e_k = r_k - engine_rpm_ave;

  // check brake
  // brake = digitalRead(brake_pin);

  // compute control signal
  u_k = Kp*e_k;
  // u_k += brake*Kff;

  // constrain for launch and PWM limits
  if (gearbox_rpm_ave < GB_LAUNCH && engine_rpm_ave >= EG_TORQUE) {
    u_k = constrain(u_k, U_K_LAUNCH_FAST, 0);
  } else if (gearbox_rpm_ave < GB_LAUNCH && engine_rpm_ave >= EG_LAUNCH) {
    u_k = constrain(u_k, U_K_LAUNCH_SLOW, 0);
  } else {
    u_k = constrain(u_k, U_K_MIN, U_K_MAX);
  }

  // force shift out if rpm is too low
  if (engine_rpm_ave < EG_KILL) {
    u_k = U_K_MAX;
  }

  // change pot outer limit
  if (engine_rpm_ave >= EG_LAUNCH) {
    pot_lim_out = POT_ENGAGE;
  } else {
    pot_lim_out = POT_MAX;
  }

  // software limit switches
  if (current_pos >= pot_lim_out) {
    u_k = -U_K_LIMIT;
  } else if (current_pos <= pot_lim_in) {
    u_k = U_K_LIMIT;
  }

  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);
}

void loop() {
  unsigned long current_micros = micros();

  // update pot position
  current_pos = analogRead(pot_pin);

  // check engine_rpm
  int reading = analogRead(engine_pin);
  if (reading > HF_HIGH) {
    im_high = true;
  }
  if (im_high && (reading < HF_LOW)) {
    engine_trigger_time = current_micros;
    engine_rpm = 60000000.0 / (engine_trigger_time - engine_last_trigger);
    engine_readings[engine_index] = engine_rpm;
    engine_index = (engine_index + 1) % num_readings;
    engine_last_trigger = engine_trigger_time;
    im_high = false;
  }

  // check gearbox rpm
  if (digitalRead(gbPin) == 1 && gbstate != 1) {
    gbRPM = (5510204.0*2)/(current_micros - gbPrevMil);
    gbPrevMil = current_micros;
    gearbox_readings[gearbox_index] = gbRPM;
    gearbox_index = (gearbox_index + 1) % num_readings;
    gbstate = 1;
  }
  if (digitalRead(gbPin) == 0) {
    gbstate = 0;
  }
}
