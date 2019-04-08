 // ecvt_lead_lag_compensator.ino
// this sketch runs a lag compensator on the ecvt
//
// author: Tyler McCown (tylermccown@engineering.ucla.edu)
// created: 4/2/19 
//
// tweaked by Iou Sheng Chang (iouschang@engineering.ucla.edu)
// tweak date: 4/5/19

#include <Wire.h>
#include <Servo.h>

// display setup
//#include <Wire.h>
//#include <I2C_LCD.h>
//I2C_LCD LCD;
//uint8_t I2C_LCD_ADDRESS = 0x51;
//extern GUI_Bitmap_t bmBruinRacing;
//extern GUI_Bitmap_t bmBearHead;
//const int refreshPeriod = 1000; // display refresh time [ms]
//int lastRefreshTime(0);

// buttons
const byte button1_pin = 13;
const byte button2_pin = 12;
const byte button3_pin = 11;
const byte button4_pin = 10;
int mode = 0;
char mode_names[3][7] = {"TORQUE", "POWER", "LAUNCH"};

// PWM constants
#define PW_STOP 1510
#define PW_MIN 1000
#define PW_MAX 2000
#define u_k_limit 100
const int u_k_min = PW_MIN - PW_STOP;
const int u_k_max = PW_MAX - PW_STOP;

// reference signals
#define MAX_TORQUE 2750
#define MAX_POWER 3600

// actuator
Servo Actuator;
const byte actuator_pin = 9;
#define POT_MARGIN 10
int POT_MIN = 125 + POT_MARGIN;
int POT_MAX = 530 - POT_MARGIN;
int POT_ENGAGE = 509;
const byte pot_pin = A0;
int current_pos(0);

// controller
int r_k = MAX_TORQUE;
int e_k(0);
int e_k1(0);
int lead_u_k(0);
int lead_u_k1(0);
int u_k(0);
int u_k1(0);
const byte controlPeriod = 20; // [ms]
const double Ts = controlPeriod/1000.0; // controlPeriod [s]
const byte K = 25; // controller gain
const double lead_z = 4.4212; // lead compensator zero
const double lead_p = 61.579; // lead compensator pole
const double lead_A = K*(lead_z+2/Ts)/(lead_p+2/Ts); // multiplied by e_k
const double lead_B = K*(lead_z-2/Ts)/(lead_p+2/Ts); // multiplied by e_k1
const double lead_C = (lead_p-2/Ts)/(lead_p+2/Ts); // multiplied by lead_u_k1
const double lag_z = 8.2424; // lag compensator zero
const double lag_p = 1.0919; // lag compensator pole
const double lag_A = (lag_z+2/Ts)/(lag_p+2/Ts); // multiplied by lead_u_k
const double lag_B = (lag_z-2/Ts)/(lag_p+2/Ts); // multiplied by lead_u_k1
const double lag_C = (lag_p-2/Ts)/(lag_p+2/Ts); // multiplied by u_k1
unsigned long lastControlTime(0);

// hall effect sensor
#define NUM_MAGNETS 1
#define sensor_pin 6
unsigned long trigger_time(0);
unsigned long last_trigger(0);
unsigned long delta_t(0);
unsigned int rpm(0);
double HighLow(0);

void setup() {

  // open serial connection
  Serial.begin(9600);

  // init I2C interface
//  Wire.begin();
//  LCD.WorkingModeConf(OFF, ON, WM_BitmapMode);

  // clear screen and display sweaty bruin
//  LCD.CleanAll(WHITE);
//  LCD.DrawScreenAreaAt(&bmBruinRacing, 0, 1);
//  delay(1000);
  
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

  // configure LCD to write text
//  LCD.CleanAll(WHITE);
//  LCD.DrawScreenAreaAt(&bmBearHead, 0, 0);
//  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
//  LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_NO_BAC);
//  LCD.DispStringAt("MODE:", 68, 5);
//  LCD.DispStringAt("RPM:", 68, 35);
//  LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);

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

    Serial.print(r_k);
    Serial.print(" ");
    Serial.print(u_k);
    Serial.print(" ");
    Serial.print(rpm);
    Serial.print(" ");
    Serial.print(current_pos);
//    Serial.print(" ");
//    Serial.print(millis());
    Serial.print("\n");
  }
}

void control_function() {
  
  // compute error
  e_k = r_k - rpm;

  // compute control signal
  lead_u_k = lead_A*e_k + lead_B*e_k1 - lead_C*lead_u_k1;
  u_k = lag_A*lead_u_k + lag_B*lead_u_k1 - lag_C*u_k1;
//  u_k = max(min(u_k, u_k_max), u_k_min);
  u_k = constrain(u_k, u_k_min, u_k_max);

  if (rpm < MAX_TORQUE) {
    POT_MAX = 530 - POT_MARGIN;
  } else {
    POT_MAX = POT_ENGAGE + POT_MARGIN;
  }

  // check actuator limits
  current_pos = analogRead(pot_pin);
  if (current_pos >= POT_MAX) {
    u_k = -u_k_limit;
  } else if (current_pos <= POT_MIN) {
    u_k = u_k_limit;
  }

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
      trigger_time = millis();
      unsigned short new_rpm = 60000.0 / (trigger_time - last_trigger);
      if (new_rpm >= 0 && new_rpm < 5000) {
        rpm = new_rpm;
      }
      last_trigger = trigger_time;
    }
    HighLow = 0;
  }
  if (digitalRead(sensor_pin) == HIGH) {
    HighLow = 1;
  }
}

//void update_display() {
//  LCD.WorkingModeConf(OFF, ON, WM_BitmapMode);
//  LCD.DrawScreenAreaAt(&bmBearHead, 0, 0);
//  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
//  LCD.DispStringAt("MODE:", 68, 5);
//  LCD.DispStringAt("RPM:", 68, 35);
//  LCD.DispStringAt(mode_names[mode], 78, 15);
//  char rpm_str[5];
//  itoa(rpm, rpm_str, 10);
//  LCD.DispStringAt(rpm_str, 78, 45);
//}

void loop() {

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

//  int current_millis = millis();
//  if (current_millis - lastRefreshTime >= refreshPeriod) {
//    update_display();
//    lastRefreshTime = current_millis;
//  }
}
