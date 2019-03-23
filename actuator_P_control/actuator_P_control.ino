#include <Servo.h>

#define ZERO_VEL 1460
#define MAX_VEL 2000
#define MIN_VEL 1000
#define MIN_V 0
#define MAX_V 5

#define POT_MIN 150
#define POT_MAX 450
#define POT_MID (POT_MAX+POT_MIN)/2

Servo Actuator;

// gain value
const int Kp = 50;

// pins
const int actuator_pin = 9;
const int pot_pin = A0;

// position values
int pos_ref(0);
int pos_current(0);
int error(0);
int error_last(0);
double d_error(0);
int uk(0);
int pulse_width(0);

int start_time(0);

#define controlPeriod 20 // controller period in milliseconds
int lastControlTime(0);

int readPotentiometer(int n_readings = 5) {
  int reading = 0;
  for (int i = 0; i < n_readings; i++) {
    reading += analogRead(pot_pin);
  }
  return(reading/n_readings);
}

int posFunction() {
  return 150*sin(2*PI/5/1000.*(millis() - start_time)) + POT_MID;
}

void setup() {
  pinMode(A0, INPUT);
  Actuator.attach(actuator_pin);
  Serial.begin(9600);
  start_time = millis()/1000;

  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
  interrupts();
}

SIGNAL(TIMER0_COMPA_vect) {
  int currentMillis = millis();
  if (currentMillis - lastControlTime >= controlPeriod) {
//    control_function();
//    lastControlTime = currentMillis;
  }
}

void control_function() {
    // get reading and calculate error
  pos_ref = POT_MID;
//  pos_ref = posFunction();
  Serial.print(pos_ref);
//  pos_current = readPotentiometer(32);
  pos_current = analogRead(pot_pin);
  error = pos_ref - pos_current;
  Serial.print(", ");
  Serial.println(pos_current);

  // error to pulse width
  uk = Kp*error + ZERO_VEL;
  pulse_width = min(max(uk, MIN_VEL), MAX_VEL);
//  Serial.println(pulse_width);
  Actuator.writeMicroseconds(pulse_width);
//  Actuator.writeMicroseconds(ZERO_VEL);
}

void loop() {
  // no need for control function in the loop function. triggered automatically by timer interrupt
  // control_function();
  // delayMicroseconds(20e3);
}

