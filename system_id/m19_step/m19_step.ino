/* m19_step.ino
 * 
 * gathers step response data for a system identification of PWM to actuator
 * position for the Thomson actuator.
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
int r_k = 220;
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
const byte button_pin2 = 10;
byte mode = 0;

void setup() {
  
  // connect to serial
//  Serial.begin(9600);

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);

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
      // set poisition reference to outer limit
      r_k = 220;
    } else if (mode == 1) {
      // set position reference to inner limit
      r_k = 170;
    }
    control_function();

//    Serial.println(analogRead(pot_pin));

    // update last run time
    lastControlTime = current_millis;
  }
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

void loop() {
  if (digitalRead(button_pin) == LOW) {
    mode = 1;
  }
  if (digitalRead(button_pin2) == LOW) {
    mode = 0;
  }
}
