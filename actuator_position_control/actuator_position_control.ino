#include <Servo.h>

// create actuator object
Servo Actuator;
const int actuator_pin = 9;

// define controller constants
const int controlPeriod = 20; // [ms]
int lastControlTime(0);
const double Kp = 4;
const double Ts = double(controlPeriod) / 1000; // controlPeriod [s]
//const double tau_f = 1/22; // anti-alias filter coefficient for 22Hz
//const double alpha = 1/(1+tau_f/Ts);

// define potentiometer parameters
#define POT_MARGIN 100
#define POT_MIN 105 + POT_MARGIN
#define POT_MAX 920 - POT_MARGIN
#define POT_MID (POT_MIN + POT_MAX)/2
const int pot_pin = A0;
int y_k(0);
int y_k1(0);

// define controller error and output values
int r_k = POT_MAX;
int e_k(0);
int u_k(0);
int u_k_final(0);

// define PWM limits
#define PW_STOP 1460
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_max = PW_MAX - PW_STOP;
const int u_k_min = PW_MIN - PW_STOP;

void setup() {
  
  // connect to serial
  Serial.begin(9600);

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);
  y_k1 = analogRead(pot_pin);

  delay(5000);

  // create timer interrupt
  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);

  // create button interrupt
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), switch_reference, FALLING);
  interrupts();
}

void switch_reference() {
  if (r_k == POT_MIN) {
    r_k = POT_MAX;
  } else {
    r_k = POT_MIN;
  }
}

SIGNAL(TIMER0_COMPA_vect) {

  // check current clock time
  int current_millis = millis();

  // if enough time has elapsed, run the control function
  if (current_millis - lastControlTime >= controlPeriod) {
    control_function();

    // update last run time
    lastControlTime = current_millis;
  }
}

void control_function() {

  // get reading and calculate error
  //y_k = alpha*analogRead(pot_pin) + (1-alpha)*y_k1;
  y_k = analogRead(pot_pin);
  e_k = r_k - y_k;

  // compute control signal
  u_k = Kp*e_k;

  // constrain final output
  u_k_final = max(min(u_k, u_k_max), u_k_min);
  
  // add controller bias and set output PWM
  Actuator.writeMicroseconds(u_k_final + PW_STOP);

  // update past values
  //y_k1 = y_k;

  Serial.print(y_k);
  Serial.print(" ");
  Serial.print(u_k_final);
  Serial.print("\n");
}

void loop() {

}

