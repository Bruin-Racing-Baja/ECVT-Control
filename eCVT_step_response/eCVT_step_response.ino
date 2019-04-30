/* ecvt_step_response.ino
 * this sketch is gathers step response data for a system identification of actuator position to engine rpm
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 3/28/19
 */

//#include <Wire.h>
//#include <I2C_LCD.h>
#include <Servo.h>

// display
//I2C_LCD LCD;
//uint8_t I2C_LCD_ADDRESS = 0x51;
//const int displayRefreshPeriod = 100; // display refresh time [ms]
//int lastDisplayRefreshTime(0);

// buttons
#define DEBOUNCE_DELAY 50
const int button1_pin = 13;
int button1_state = HIGH;
int button1_last_state = HIGH;
int button1_last_debounce(0);
const int button2_pin = 12;
int button2_state = HIGH;
int button2_last_state = HIGH;
int button2_last_debounce(0);
const int button3_pin = 11;
int button3_state = HIGH;
int button3_last_state = HIGH;
int button3_last_debounce(0);
const int button4_pin = 10;
int button4_state = HIGH;
int button4_last_state = HIGH;
int button4_last_debounce(0);

// PWM constants
#define PW_STOP 1510
#define PW_MIN 1000
#define PW_MAX 2000
const int u_k_min = PW_MIN - PW_STOP;
const int u_k_max = PW_MAX - PW_STOP;

// actuator
Servo Actuator;
const int actuator_pin = 9;
#define POT_MARGIN 10
#define POT_MIN 146 + POT_MARGIN
#define POT_MAX 762 - POT_MARGIN
const int pot_pin = A5;
int current_pos(0);
int u_k(0);
const int controlPeriod = 0; // [ms]
int lastControlTime(0);

// hall effect sensor
#define NUM_MAGNETS 1
const int sensor_pin = 7;
//int trigger_time(0);
//int last_trigger(0);
//int rpm(0);
//int rpm_count = 0;
int rpm(0);
int last_trigger(0);
int trigger_time(0);
int HighLow = LOW;
bool RPMCount = false;

void setup() {

  // connect to serial
//  Serial.begin(9600);

  // setup display
//  Wire.begin();
  // configure LCD for boot logo OFF, backlight ON, character display mode
//  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
//  LCD.CleanAll(WHITE);
//  LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
//  LCD.DispStringAt("STEP: ", 20, 20);
//  LCD.DispStringAt("RPM: ", 20, 40);

  // setup buttons
//  pinMode(button1_pin, INPUT);
//  pinMode(button2_pin, INPUT);
//  pinMode(button3_pin, INPUT);
//  pinMode(button4_pin, INPUT);

  // setup actuator
  Actuator.attach(actuator_pin);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  current_pos = analogRead(pot_pin);

  // create hall effect interrupt
  pinMode(sensor_pin, INPUT);
//  attachInterrupt(digitalPinToInterrupt(sensor_pin), hall_effect_interrupt, FALLING);
//  interrupts();
  trigger_time = millis();

  delay(5000);
  u_k = u_k_min;

  // create timer interrupt
//  OCR0A = 0xFF;
//  TIMSK0 |= _BV(OCIE0A);
}

//void hall_effect_interrupt() {
//  last_trigger = trigger_time;
//  trigger_time = millis();
//}

//SIGNAL(TIMER0_COMPA_vect) {
//  int current_millis = millis();
//  if (current_millis - lastControlTime >= controlPeriod) {
//    RPMCalc();
//    Actuator.writeMicroseconds(u_k + PW_STOP);
//    lastControlTime = current_millis;
//
//    Serial.print(u_k);
//    Serial.print(" ");
//    Serial.print(rpm);
//    Serial.print(" ");
//    Serial.print(current_pos);
//    Serial.print(" ");
//    Serial.print(millis());
////    Serial.print(" ");
////    Serial.print(button1_state);
////    Serial.print(" ");
////    Serial.print(button2_state);
////    Serial.print(" ");
////    Serial.print(button3_state);
////    Serial.print(" ");
////    Serial.print(button4_state);
//    Serial.print("\n");
//  }
//}

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
    rpm = (60000.0 / (trigger_time - last_trigger));
    RPMCount = 0;
    last_trigger = trigger_time;
  }
}

void loop() {
    int current_millis = millis();
    RPMCalc();
    Actuator.writeMicroseconds(u_k + PW_STOP);
    lastControlTime = current_millis;

  if (current_millis - lastControlTime >= controlPeriod) {
    Serial.print(u_k);
    Serial.print(" ");
    Serial.print(rpm);
    Serial.print(" ");
    Serial.print(current_pos);
    Serial.print(" ");
    Serial.print(millis());
//    Serial.print(" ");
//    Serial.print(button1_state);
//    Serial.print(" ");
//    Serial.print(button2_state);
//    Serial.print(" ");
//    Serial.print(button3_state);
//    Serial.print(" ");
//    Serial.print(button4_state);
    Serial.print("\n");
  }
  
  // check actuator limits
  current_pos = analogRead(pot_pin);
//  LCD.DispStringAt(current_pos, 0, 60);
  if (((current_pos >= POT_MAX) && (u_k > 0)) || ((current_pos <= POT_MIN) && (u_k < 0))) {
    u_k = 0;
//    LCD.DispStringAt("   ", 68, 20);
  }

  // check button presses
//  debounce_button(digitalRead(button1_pin), button1_state, button1_last_state, button1_last_debounce); 
//  debounce_button(digitalRead(button2_pin), button2_state, button2_last_state, button2_last_debounce);
//  debounce_button(digitalRead(button3_pin), button3_state, button3_last_state, button3_last_debounce);
//  debounce_button(digitalRead(button4_pin), button4_state, button4_last_state, button4_last_debounce);
  
//  if (button1_state == LOW) {
//    u_k = u_k_min;
////    LCD.DispStringAt("IN ", 68, 20);
//  }
//  if (button2_state == LOW) {
//    u_k = u_k_max;
////    LCD.DispStringAt("OUT", 68, 20);
//  }
//  if (button3_state == LOW) {
////    LCD.DispStringAt("Taiwan #1 ", 0, 0);
//    u_k = 0;
////    LCD.DispStringAt("   ", 68, 20);
//  }
//  if (button4_state == LOW) {
////    LCD.DispStringAt("send nudes", 0, 0);
//    u_k = 0;
////    LCD.DispStringAt("   ", 68, 20);
//  }

  // calculate rpm
//  detachInterrupt(digitalPinToInterrupt(sensor_pin));
//  rpm = 60000.0/NUM_MAGNETS/(trigger_time - last_trigger);
////  if (new_rpm > 0) {
////    rpm = new_rpm;
////  }
//  attachInterrupt(digitalPinToInterrupt(sensor_pin), hall_effect_interrupt, FALLING);
  
  // update display rpm at appropriate interval
//  int current_time = millis();
//  if (current_time - lastDisplayRefreshTime >= displayRefreshPeriod) {
//    LCD.CharGotoXY(60, 40);
//    LCD.print(rpm);
//      lastDisplayRefreshTime = current_time;
//  }
}

//bool debounce_button(const int reading, int &button_state, int &last_button_state , int &last_debounce) {
//  if (reading != last_button_state) {
//    last_debounce = millis();
//  }
//
//  if ((millis() - last_debounce) > DEBOUNCE_DELAY) {
//
//    if (reading != button_state) {
//      button_state = reading;
//    }
//  }
//
//  last_button_state = reading;
//}
