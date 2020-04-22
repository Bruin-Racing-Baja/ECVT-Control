/* m19_chirp.ino
 * 
 * gathers chirp response data for a system identification of PWM to engine rpm
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 8 May 2019
 */

#include <Servo.h>

// create actuator object
Servo Actuator;
const int actuator_pin = 9;

// define controller constants
const int controlPeriod = 20; // [ms]
int lastControlTime(0);
const double Kp = 20;
const double Ts = double(controlPeriod) / 1000; // controlPeriod [s]

// define potentiometer parameters
#define POT_MIN 167
#define POT_MAX 294
#define POT_ENGAGE 245
const int pot_pin = A1;
int y_k(0);

// define controller error and output values
int r_k = 245;
int e_k(0);
int u_k(0);
int u_k_final(0);

// define PWM limits
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_max = PW_MAX - PW_STOP;
const int u_k_min = PW_MIN - PW_STOP;

// running mode update stuff
const byte button_pin = 13;
byte mode = 0;
unsigned long chirp_start_time(0);

// chirp parameters
double A = 485;     // amplitude [servo]
double F_MIN = 0.2; // starting freq [Hz]
double F_MAX = 2; // final freq [Hz]
double T = 10;      // period [s]

void setup() {
  
  // connect to serial
//  Serial.begin(9600);

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);

  // create button interrupt
//  attachInterrupt(digitalPinToInterrupt(button_pin), switch_mode, FALLING);
//  interrupts();

  // create timer interrupt
  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) {

  // check current clock time
  int current_millis = millis();

  // if enough time has elapsed, run the control function
  if (current_millis - lastControlTime >= controlPeriod) {
    
    if (mode == 0) {
      // position controller
      control_function();
    } else if (mode == 1) {
      // chirp response
      chirp_function();
    }

//    Serial.println(analogRead(pot_pin));

    // update last run time
    lastControlTime = current_millis;
  }
}

void switch_mode() {
  mode = 1;
  chirp_start_time = micros();
}

void control_function() {

  // get reading and calculate error
  y_k = analogRead(pot_pin);
  e_k = r_k - y_k;

  // compute control signal
  u_k = Kp*e_k;

  // constrain final output
  u_k_final = max(min(u_k, u_k_max), u_k_min);
  
  // add controller bias and set output PWM
  Actuator.writeMicroseconds(u_k_final + PW_STOP);
}

void chirp_function() {
  double t = (micros() - chirp_start_time) * pow(10,-6);
  u_k = -A*sin(((2*PI*F_MIN*T)/(log(F_MAX/F_MIN)))*(exp(((t)*log(F_MAX/F_MIN))/T) - 1)) + PW_STOP;
  Actuator.writeMicroseconds(u_k);
  if (t > 10) {
    mode = 0;
  }
}

void loop() {
  if (digitalRead(button_pin) == LOW) {
    mode = 1;
    chirp_start_time = micros();
  }
}
