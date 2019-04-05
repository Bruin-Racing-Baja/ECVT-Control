// ecvt_lead_lag_compensator.ino
// this sketch runs a lag compensator on the ecvt
//
// author: Tyler McCown (tylermccown@engineering.ucla.edu)
// created: 4/2/19 
//
// tweaked by Iou Sheng Chang (iouschang@engineering.ucla.edu)
// tweak date: 4/5/19


#include <Servo.h>

// buttons
const byte button1_pin = 13;
const byte button2_pin = 12;
const byte button3_pin = 11;
const byte button4_pin = 10;

// PWM constants
#define PW_STOP 1510
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_min = PW_MIN - PW_STOP;
const int u_k_max = PW_MAX - PW_STOP;

// reference signals
#define MAX_TORQUE 2700
#define MAX_POWER 3400

// actuator
Servo Actuator;
const byte actuator_pin = 9;
#define POT_MARGIN 10
#define POT_MIN 146 + POT_MARGIN
#define POT_MAX 762 - POT_MARGIN
const byte pot_pin = A0;
int current_pos(0);

// controller
int u_k(0);
int u_k1(0);
int lead_u_k1(0);
int lead_u_k(0);
int e_k(0);
int e_k1(0);
int r_k = MAX_TORQUE;
const byte controlPeriod = 20; // [ms]
const double Ts = controlPeriod/1000; // controlPeriod [s]
const byte K = 5; // controller gain
const double lead_z = 5.602; // lead compensator zero
const double lead_p = 11.43; // lead compensator pole
const double lead_A = K*(lead_z+2/Ts)/(lead_p+2/Ts); // multiplied by e_k
const double lead_B = K*(lead_z-2/Ts)/(lead_p+2/Ts); // multiplied by e_k1
const double lead_C = (lead_p-2/Ts)/(lead_p+2/Ts); // multiplied by lead_u_k1
const double lag_z = .9487; // lag compensator zero
const double lag_p = .09487; // lag compensator pole
const double lag_A = (lag_z+2/Ts)/(lag_p+2/Ts); // multiplied by lead_u_k
const double lag_B = (lag_z-2/Ts)/(lag_p+2/Ts); // multiplied by lead_u_k1
const double lag_C = (lag_p-2/Ts)/(lag_p+2/Ts); // multiplied by u_k1
unsigned int lastControlTime(0);

// hall effect sensor
#define NUM_MAGNETS 1
#define sensor_pin 6
double trigger_time(0);
double last_trigger(0);
double delta_t(0);
double rpm(0);
double HighLow(0);
double RPMCount(0);

void setup() {
  // setup buttons
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  current_pos = analogRead(pot_pin);

  // create hall effect interrupt
//  pinMode(sensor_pin, INPUT);
//  attachInterrupt(digitalPinToInterrupt(sensor_pin), hall_effect_interrupt, FALLING);
//  interrupts();
//  trigger_time = millis();
//  last_trigger = trigger_time;

  // create timer interrupt
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

//void hall_effect_interrupt() {
//  last_trigger = trigger_time;
//  trigger_time = millis();
//}

SIGNAL(TIMER0_COMPA_vect) {
  int current_millis = millis();
  if (current_millis - lastControlTime >= controlPeriod) {
    control_function();
    lastControlTime = current_millis;

    Serial.print(u_k);
    Serial.print(" ");
    Serial.print(rpm);
    Serial.print(" ");
    Serial.print(current_pos);
    Serial.print(" ");
    Serial.print(millis());
    Serial.print("\n");
  }
}

void control_function() {
  // compute rpm
//  RPMCalc();

  // compute error
  e_k = r_k - rpm;

  // compute control signal
  lead_u_k = lead_A*e_k + lead_B*e_k1 - lead_C*lead_u_k1;
  u_k = lag_A*lead_u_k + lag_B*lead_u_k1 - lag_C*u_k1;
//  u_k = max(min(u_k, u_k_max), u_k_min);
  u_k = constrain(u_k,u_k_min,u_k_max); 

  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);

  // update past values
  e_k1 = e_k;
  lead_u_k1 = lead_u_k;
  u_k1 = u_k;
}

void RPMCalc() {
  if (digitalRead(sensor_pin) == LOW) {
    if (HighLow == 1) {
      RPMCount = 1;
    }
    HighLow = 0;
  }
  if (digitalRead(sensor_pin) == HIGH) {
    HighLow = 1;
  }
  if (RPMCount == 1) {
    trigger_time = millis();
    rpm = 60000.0 / (trigger_time - last_trigger);
    RPMCount = 0;
    last_trigger = trigger_time;
  }
}

void loop() {
  // check actuator limits
  current_pos = analogRead(pot_pin);
  if ((current_pos >= POT_MAX && u_k > 0) || (current_pos <= POT_MIN && u_k < 0)) {
    u_k = 0;
  }

  // check rpm
  RPMCalc();

  // check button presses
  if (digitalRead(button1_pin) == LOW) {
    r_k = MAX_TORQUE;
  }
  if (digitalRead(button2_pin) == LOW) {
    r_k = MAX_POWER;
  }
  if (digitalRead(button3_pin) == LOW) {
    r_k = 5000;
  }
}
