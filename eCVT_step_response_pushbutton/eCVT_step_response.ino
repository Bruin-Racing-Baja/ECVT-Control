#include <Servo.h>

// data
String data = "";

// create actuator object
Servo Actuator;
const int actuator_pin = 9; 

// define controller constants
const int controlPeriod = 20; // [ms]
int lastControlTime(0);
const double Kp = 20;
const double Ts = double(controlPeriod) / 1000; // controlPeriod [s]

// engine sensor
#define HF_HIGH 800
#define HF_LOW 200
bool im_high = false;
const byte engine_pin = A3;
unsigned long engine_trigger_time(0);
unsigned long engine_last_trigger(0);
unsigned int engine_rpm(0);

// moving average filters
const size_t num_readings = 4;
unsigned int engine_rpm_ave(0);
byte engine_index = 0;
unsigned int engine_readings[num_readings];

// define potentiometer parameters
#define POT_MIN 167
#define POT_MAX 294
#define POT_ENGAGE 245
const int pot_pin = A0;
int y_k(0);

// define controller error and output values
int r_k = 240;
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
unsigned long step_start_time(0);

void setup() {
  // connect to serial
  Serial.begin(9600);

  // attach actuator and limit write commands
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);
  Actuator.writeMicroseconds(PW_STOP);

  // setup engine sensor
  pinMode(engine_pin, INPUT);
  init_readings(engine_readings);

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
      // step response
      step_function();
    }

    writeData();
    
    // update last run time
    lastControlTime = current_millis;
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
  // calculate engine rpm
  engine_rpm_ave = rpm_average(engine_readings);
  
  // get reading and calculate error
  y_k = analogRead(pot_pin);
  e_k = r_k - y_k;

  // compute control signal
  u_k = Kp*e_k;

  // constrain final output
  u_k_final = constrain(u_k,u_k_min,u_k_max) + PW_STOP;
  
  // add controller bias and set output PWM
  Actuator.writeMicroseconds(u_k_final);
}

void step_function() {
  // get reading
  engine_rpm_ave = rpm_average(engine_readings);
  y_k = analogRead(pot_pin);
  
  double t = (micros() - step_start_time) * pow(10,-6);
  u_k_final = PW_MIN;
  Actuator.writeMicroseconds(u_k_final);
  if (t > 5) {
    mode = 0;
  }
}

void writeData(){
  data += u_k_final;
  data += ", ";
  data += engine_rpm_ave;
  data += ", ";
//  data += y_k;
  data += analogRead(pot_pin);
  Serial.println(data);
  data = "";
}

void loop() {
  if (digitalRead(button_pin) == LOW) {
    mode = 1;
    step_start_time = micros();
  }

  // check engine_rpm
  int reading = analogRead(engine_pin);
  if (reading > HF_HIGH) {
    im_high = true;
  }
  if (im_high && (reading < HF_LOW)) {
    engine_trigger_time = micros();
    if (0 <= engine_rpm <= 4000){
      engine_rpm = 60000000.0 / (engine_trigger_time - engine_last_trigger);
    }
    engine_readings[engine_index] = engine_rpm;
    engine_index = (engine_index + 1) % num_readings;
    engine_last_trigger = engine_trigger_time;
    im_high = false;
  }
}
