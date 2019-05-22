/* eCVT_chirp_response.ino
 * this sketch gathers chirp response data for a system identification of PWM to engine rpm
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 5/18/19
 */

#include <Servo.h>

// create actuator object
Servo Actuator;
const int actuator_pin = 9;

// define controller constants
const int control_period = 20; // [ms]
const double Kp = 20;
const double Ts = double(control_period) / 1000; // controlPeriod [s]

// define potentiometer parameters
#define POT_MIN 165
#define POT_MAX 294
#define POT_ENGAGE 245
const int pot_pin = A1;
int y_k(0);
int current_pos(0);

// define controller error and output values
int r_k = 225;
int e_k(0);
int u_k(0);
int u_k_final(0);
unsigned long last_control_time(0);

// define PWM limits
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_max = PW_MAX - PW_STOP;
const int u_k_min = PW_MIN - PW_STOP;

// running mode update stuff
bool mode = 0;
unsigned long last_change_time(0);
const int switch_period = 5000; // ms

void setup() {
  
  // connect to serial
  Serial.begin(9600);

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);
}

void control_function() {

  // get reading and calculate error
  y_k = analogRead(pot_pin);
  e_k = r_k - y_k;

  // compute control signal
  u_k = Kp*e_k;

  // constrain final output
  u_k_final = constrain(u_k, u_k_min, u_k_max);
  
  // add controller bias and set output PWM
  Actuator.writeMicroseconds(u_k_final + PW_STOP);
}

void loop() {

  unsigned long current_time = millis();
  
  // switch reference value
  if (current_time - last_change_time >= switch_period) {
    if (mode == 0) {
      // quarter shift
      r_k = 225;
    } else if (mode == 1) {
      // half shift
      r_k = 185;
    }
    mode = !mode;
    last_change_time = current_time;
  }

  // control loop
  if (current_time - last_control_time >= control_period) {
    current_pos = analogRead(pot_pin);
    control_function();
    last_control_time = current_time;
  }

  Serial.println(r_k);
}
