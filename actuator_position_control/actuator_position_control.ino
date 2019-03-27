#include <Servo.h>

// create actuator object
Servo Actuator;
const int actuator_pin = 9;

// define potentiometer parameters
#define POT_MIN 105
#define POT_MAX 920
#define POT_MID (POT_MIN + POT_MAX)/2
const int pot_pin = A0;
int current_pos(0);

// define controller constants
const int controlPeriod = 20; // [ms]
int lastControlTime(0);
const double Kp = 15.3;
const double Ki = 27;
const double Kd = .29;
const double Kb = 3;
const double Ts = double(controlPeriod) / 1000; // controlPeriod [s]
const double A = Kp + Ki * Ts / 2 + Kd / Ts;
const double B = -Kp + Ki * Ts / 2 - 2 / Ts * Kd;
const double C = Kd / Ts;

// define controller error and output values
int r_k = POT_MID;
int u_k(0);
int u_k_final(0);
int u_k1(0);
int e_k(0);
int e_k1(0);
int e_k2(0);

// define PWM limits
#define PW_STOP 1460
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_max = PW_MAX - PW_STOP;
const int u_k_min = PW_MIN - PW_STOP;

void setup() {
  // create timer interrupt
  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
  interrupts();

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);

  // connect to serial
  Serial.begin(9600);
}

SIGNAL(TIMER0_COMPA_vect) {

  // check current clock time
  int current_millis = millis();

  // if enough time has elapsed, run the control function
  if (current_millis - lastControlTime <= controlPeriod) {
    control_function();

    // update last run time
    lastControlTime = current_millis;
  }
}

void control_function() {

  // get reading and calculate error
  current_pos = analogRead(pot_pin);
  e_k = r_k - current_pos;

  // calculate output value and constrain
  u_k = u_k1 + A * e_k + B * e_k1 + C * e_k2;

  // clamp output signal and implement back calculation
  u_k_final = max(min(u_k, u_k_max), u_k_min);
//  e_k -= Kb * (u_k - u_k_final);

  // set output PWM
  Actuator.writeMicroseconds(u_k_final + PW_STOP);

  // update past values
  u_k1 = u_k_final;
  e_k2 = e_k1;
  e_k1 = e_k;
}

void loop() {
  Serial.print(current_pos);
  Serial.print(" ");
  Serial.print(e_k);
  
//  Serial.print(A);
//  Serial.print(", ");
//  Serial.print(B);
//  Serial.print(", ");
//  Serial.print(C);
//  Serial.print(", ");
//  Serial.print(Ts);

  Serial.print("\n");
}

