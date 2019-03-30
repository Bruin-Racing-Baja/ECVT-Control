// eCVT_step_response.ino
// this program is meant for gathering step response data from the powertrain
//
// author: Tyler McCown (tylermccown@engineering.ucla.edu)
// created: 3/28/19

#include <Wire.h>
#include <I2C_LCD.h>
#include <Servo.h>

// display
I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51;
const int displayRefreshPeriod = 100; // display refresh time [ms]
int lastDisplayRefreshTime(0);

// buttons
const int button1_pin = 13;
const int button2_pin = 12;
const int button3_pin = 11;
const int button4_pin = 10;

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
const int controlPeriod = 20; // [ms]
int lastControlTime(0);

// hall effect sensor
#define NUM_MAGNETS 1
const int sensor_pin = 7;
int trigger_time(0);
int last_trigger(0);
int rpm(0);
int rpm_count = 0;

void setup() {

  // connect to serial
//  Serial.begin(9600);

  // setup display
//  Wire.begin();
  // configure LCD for boot logo OFF, backlight ON, character display mode
//  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
//  LCD.CleanAll(WHITE);
//  LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
//  LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_BAC);
//  LCD.DispStringAt("STEP: ", 20, 20);
//  LCD.DispStringAt("RPM: ", 20, 40);

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
  pinMode(sensor_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor_pin), hall_effect_interrupt, FALLING);
  interrupts();
  trigger_time = millis();
  last_trigger = trigger_time;

  // create timer interrupt
  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
}

void hall_effect_interrupt() {
  last_trigger = trigger_time;
  trigger_time = millis();
//  int trigger_time = millis();
//  rpm = 60000.0/NUM_MAGNETS/(trigger_time - last_trigger);
//  last_trigger = trigger_time;
}

SIGNAL(TIMER0_COMPA_vect) {
  int current_millis = millis();
  if (current_millis - lastControlTime >= controlPeriod) {
    Actuator.writeMicroseconds(u_k + PW_STOP);
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

void loop() {
  // check actuator limits
  current_pos = analogRead(pot_pin);
  if (current_pos >= POT_MAX || current_pos <= POT_MIN) {
    u_k = 0;
//    LCD.DispStringAt("   ", 68, 20);
  }
  
  // check button presses
  if (digitalRead(button3_pin) == LOW) {
    u_k = u_k_min;
//    u_k = 1400;
//    LCD.CharGotoXY(68, 20);
//    LCD.print("IN ");
  } else if (digitalRead(button4_pin) == LOW) {
    u_k = u_k_max;
//    u_k = 1600;
//    LCD.CharGotoXY(68, 20);
//    LCD.print("OUT");
//  } else if (digitalRead(button3_pin) == LOW) {
////    LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_BAC);
//    LCD.DispStringAt("send nudes", 0, 0);
////    LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
  } else if (digitalRead(button2_pin) == LOW) {
//    LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_BAC);
//    LCD.DispStringAt("Taiwan #1 ", 0, 0);
//    LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
    u_k = 0;
  }

  // calculate rpm+
  detachInterrupt(digitalPinToInterrupt(sensor_pin));
  rpm = 60000.0/NUM_MAGNETS/(trigger_time - last_trigger);
//  Serial.println(rpm);
  attachInterrupt(digitalPinToInterrupt(sensor_pin), hall_effect_interrupt, FALLING);
  
  // update display rpm at appropriate interval
//  int current_time = millis();
//  if (current_time - lastDisplayRefreshTime >= displayRefreshPeriod) {
//    LCD.CharGotoXY(60, 40);
//    LCD.print(rpm);
//  }
}
