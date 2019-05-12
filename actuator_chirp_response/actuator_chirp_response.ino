#include <Servo.h>

// create actuator object
Servo Actuator;
const int actuator_pin = 9;

// control constants
const int controlPeriod = 20; // [ms]
int lastControlTime(0);

// define potentiometer parameters
#define POT_MIN 167
#define POT_MAX 294
#define POT_ENGAGE 245
const int pot_pin = A0;
int y_k(0);
int current_pos(0);

// define PWM limits
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_max = PW_MAX - PW_STOP;
const int u_k_min = PW_MIN - PW_STOP;

// chirp parameters
double A = 485;     // amplitude [servo]
double F_MIN = 0.2; // starting freq [Hz]
double F_MAX = 2; // final freq [Hz]
double T = 10;      // period [s]
unsigned long chirp_start_time(0);
int u_k(PW_STOP);

void setup() {

  // connect to serial
  Serial.begin(9600);

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);

  chirp_start_time = micros();
  
  // create timer interrupt
  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) {

  // check current clock time
  int current_millis = millis();

  // if enough time has elapsed, run the control function
  if (current_millis - lastControlTime >= controlPeriod) {

    current_pos = analogRead(pot_pin);
    if (current_pos < POT_MAX && current_pos > POT_MIN) {
      chirp_function();
    } else {
      u_k = PW_STOP;
      Actuator.writeMicroseconds(u_k);
    }

    Serial.print(u_k);
    Serial.print(" ");
    Serial.println(current_pos);

    // update last run time
    lastControlTime = current_millis;
  }
}

void chirp_function() {
  double t = (micros() - chirp_start_time) * pow(10,-6);
  u_k = -A*sin(((2*PI*F_MIN*T)/(log(F_MAX/F_MIN)))*(exp(((t)*log(F_MAX/F_MIN))/T) - 1)) + PW_STOP;
  Actuator.writeMicroseconds(u_k);
}

void loop() {

}
