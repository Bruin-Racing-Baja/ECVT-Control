/* ecvt_PID_control.ino
 * this sketch runs a PID controller on the ecvt
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 4/27/19 
 */

#include <Wire.h>
#include <Servo.h>

// display setup
#include <Wire.h>
#include <I2C_LCD.h>
I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51;
extern GUI_Bitmap_t bmBruinRacing;
extern GUI_Bitmap_t bmBearHead;
const int refreshPeriod = 1000; // display refresh time [ms]
int lastRefreshTime(0);

// buttons
const byte button1_pin = 13;
const byte button2_pin = 12;
const byte button3_pin = 11;
const byte button4_pin = 10;
byte mode = 0;
char mode_names[2][7] = {"TORQUE", "POWER "};

// PWM constants
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
#define u_k_limit 100
const int u_k_min = PW_MIN - PW_STOP;
const int u_k_max = PW_MAX - PW_STOP;

// reference signals
#define MAX_TORQUE 2750
#define MAX_POWER 3500
#define LAUNCH 2200
#define THRESH_LOW 3000
#define THRESH_HIGH 3250

// actuator
Servo Actuator;
const byte actuator_pin = 9;
#define POT_MARGIN 10
int POT_MIN = 107 + POT_MARGIN;
int POT_MAX = 530 - POT_MARGIN;
int POT_ENGAGE = 509;
const byte pot_pin = A0;
int current_pos(0);

// "14th birthday" controller
int r_k = MAX_TORQUE;
int e_k(0);
int e_k1(0);
int e_k_sum(0);
int u_k(0);
int u_k1(0);
const byte controlPeriod = 20; // [ms]
const double Ts = controlPeriod/1000.0; // controlPeriod [s]
const double Kp = 14;
const double Ki = 2.744;
const double Kd = .626;
const int N = 100;
unsigned long lastControlTime(0);

// engine sensor
#define HF_HIGH 800
#define HF_LOW 100
bool im_high = false;
const byte engine_pin = A4;
unsigned long engine_trigger_time(0);
unsigned long engine_last_trigger(0);
unsigned int engine_rpm(0);  

// gearbox sensor
const byte gearbox_pin = 3;
unsigned long gearbox_trigger_time(0);
unsigned long gearbox_last_trigger(0);
unsigned int gearbox_rpm(0);

// moving average filters
const size_t num_readings = 4;
unsigned int engine_rpm_ave(0);
byte engine_index = 0;
unsigned int engine_readings[num_readings];
unsigned int gearbox_rpm_ave(0);
byte gearbox_index = 0;
unsigned int gearbox_readings[num_readings];

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // open serial connection
  Serial.begin(9600);

  // init I2C interface
  // Wire.begin();
  // LCD.WorkingModeConf(OFF, ON, WM_BitmapMode);

  // clear screen and display sweaty bruin
  // LCD.CleanAll(WHITE);
  // LCD.DrawScreenAreaAt(&bmBruinRacing, 0, 1);
  // delay(2000);
  
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

  // setup engine sensor
  pinMode(engine_pin, INPUT);
  init_readings(engine_readings);
  engine_last_trigger = millis();

  // setup gearbox sensor
  pinMode(gearbox_pin, INPUT);
  init_readings(gearbox_readings);
  gearbox_last_trigger = millis();
  attachInterrupt(digitalPinToInterrupt(gearbox_pin), gearbox_isr, CHANGE);
  interrupts();

  // configure LCD to write text
  // LCD.CleanAll(WHITE);
  // LCD.DrawScreenAreaAt(&bmBearHead, 0, 0);
  // LCD.WorkingModeConf(OFF, ON, WM_CharMode);
  // LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_NO_BAC);
  // LCD.DispStringAt("MODE:", 68, 5);
  // LCD.DispStringAt("RPM:", 68, 35);
  // LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);

  // create timer interrupt
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) {
  int current_millis = millis();
  if (current_millis - lastControlTime >= controlPeriod) {
    control_function();
    lastControlTime = current_millis;

    Serial.print(r_k);
    // Serial.print(" ");
    // Serial.print(u_k);
    // Serial.print(" ");
    // Serial.print(engine_rpm);
    Serial.print(" ");
    Serial.print(engine_rpm_ave);
    // Serial.print(" ");
    // Serial.print(gearbox_rpm);
    Serial.print(" ");
    Serial.print(gearbox_rpm_ave);
    Serial.print(" ");
    Serial.print(current_pos);
    // Serial.print(" ");
    // Serial.print(millis());
    Serial.print("\n");
  }
}

void gearbox_isr() {
  gearbox_trigger_time = millis();
  gearbox_rpm = 5.510204 / (gearbox_trigger_time - gearbox_last_trigger);
  gearbox_readings[gearbox_index] = gearbox_rpm;
  gearbox_index = (gearbox_index + 1) % num_readings;
  gearbox_last_trigger = gearbox_trigger_time;
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

  // calculate engine_rpm
  engine_rpm_ave = rpm_average(engine_readings);

// adjust reference
//  if (engine_rpm_ave < THRESH_LOW) {
//    r_k = MAX_TORQUE;
//  } else if (engine_rpm_ave > THRESH_HIGH) {
//    r_k = MAX_POWER;
//  }
  
  // compute error
  e_k = r_k - engine_rpm_ave;
  e_k_sum = constrain(e_k_sum + e_k, u_k_min, u_k_max);

  // compute control signal
  u_k = Kp*e_k + Ki*Ts/2*e_k_sum + u_k1 + Kd*N*(e_k-e_k1)-(N*Ts-1)*u_k1;
  u_k = constrain(u_k, u_k_min, u_k_max);

  if (engine_rpm_ave < 1000) {
    u_k = u_k_max;
  }

  // check actuator limits
  if (current_pos >= POT_MAX) {
   u_k = -u_k_limit;
  } else if (current_pos <= POT_MIN) {
   u_k = u_k_limit;
  }

  // write to actuator
  Actuator.writeMicroseconds(u_k + PW_STOP);

  // update past values
  e_k1 = e_k;
  u_k1 = u_k;
  
}

void update_display() {
  LCD.WorkingModeConf(OFF, ON, WM_BitmapMode);
  LCD.DrawScreenAreaAt(&bmBearHead, 0, 0);
  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
  LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_NO_BAC);
  LCD.DispStringAt("MODE:", 68, 5);
  LCD.DispStringAt("RPM:", 68, 35);
  LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
  LCD.DispStringAt(mode_names[mode], 78, 15);
  char rpm_str[5];
  itoa(engine_rpm, rpm_str, 10);
  LCD.DispStringAt(rpm_str, 78, 45);
}

void loop() {

  // update pot position
  current_pos = analogRead(pot_pin);

  // check engine engine_rpm
  int reading = analogRead(engine_pin);
  if (reading > HF_HIGH) {
    im_high = true;
  } 
  if (im_high && (reading < HF_LOW)) {
    engine_trigger_time = millis();
    engine_rpm = 60000.0 / (engine_trigger_time - engine_last_trigger);
    engine_readings[engine_index] = engine_rpm;
    engine_index = (engine_index + 1) % num_readings;
    engine_last_trigger = engine_trigger_time;
    im_high = false;
  }

  // check button presses
   if (digitalRead(button1_pin) == LOW) {
     r_k = MAX_TORQUE;
     mode = 0;
   }
   if (digitalRead(button2_pin) == LOW) {
     r_k = MAX_POWER;
     mode = 1;
   }

  // refresh display
  int current_millis = millis();
  if (current_millis - lastRefreshTime >= refreshPeriod) {
    // update_display();
    lastRefreshTime = current_millis;
  }

}
