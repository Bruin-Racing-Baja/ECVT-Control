/* actuator_speed_calibration.ino
 * uses a potentiometer input to control the actuator position for length measurements
 * 
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 26 October 2019
 */

#include <Servo.h>

// create actuator object
Servo Actuator;
const byte actuator_pin = 9;

// define controller constants
const int control_period = 20; // [ms]
unsigned long last_control_time(0);
const int Kp = 20;

// define potentiometer parameters
#define POT_MIN 163
#define POT_MAX 254
const byte pot_pin = A1;

// define controller error and output values
int r_k = POT_MAX;

// define PWM limits
#define PW_STOP 1460
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_max = PW_MAX - PW_STOP;
const int u_k_min = PW_MIN - PW_STOP;

// input
const byte input_pin = A0;

void setup() {
  
  // connect to serial
  Serial.begin(9600);

  // attach actuator
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);

}

void control_function() {
  // update reference
  r_k = map(analogRead(input_pin), 0, 1023, POT_MIN, POT_MAX);

  // get reading and calculate error
  int current_pos = analogRead(pot_pin);
  int e_k = r_k - current_pos;

  // compute control signal
  int u_k = Kp*e_k;

  // constrain final output
  u_k = constrain(u_k, u_k_min, u_k_max);
  
  // add controller bias and set output PWM
  Actuator.writeMicroseconds(u_k + PW_STOP);

  Serial.print(current_pos);
  Serial.print(" ");
  Serial.print(r_k);
  Serial.print("\n");
}

void loop() {
    unsigned long current_millis = millis();

    if (current_millis - last_control_time >= control_period) {
        control_function();
        last_control_time = current_millis;
    }
}
