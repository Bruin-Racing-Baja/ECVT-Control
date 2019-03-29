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
#define POT_MARGIN 50
#define POT_MIN 105 + POT_MARGIN
#define POT_MAX 920 - POT_MARGIN
const int pot_pin = A0;
int current_pos(0);
int u_k(0);
const int controlPeriod = 20; // [ms]
int lastControlTime(0);

// hall effect sensor
#define NUM_MAGNETS 1
const int sensor_pin = 7;
int last_trigger(0);
int rpm(0);

void setup() {

  // connect to serial
  Serial.begin(9600);

  // setup display
  Wire.begin();
  // configure LCD for boot logo OFF, backlight ON, character display mode
  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
  LCD.CleanAll(WHITE);
  LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
  LCD.DispStringAt("STEP: ", 20, 20);
  LCD.DispStringAt("RPM: ", 20, 40);

  // setup buttons
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);

  // setup actuator
  Actuator.attach(actuator_pin);
  pinMode(pot_pin, INPUT);
  current_pos = analogRead(pot_pin);

  // create hall effect interrupt
  pinMode(sensor_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor_pin), calc_rpm, RISING);
  interrupts();
  last_trigger = millis();

  // create timer interrupt
  OCR0A = 0xFF;
  TIMSK0 |= _BV(OCIE0A);
}

void calc_rpm() {
  int trigger_time = millis();
  rpm = 60000/NUM_MAGNETS/(trigger_time - last_trigger);
  last_trigger = trigger_time;
}

SIGNAL(TIMER0_COMPA_vect) {
  int current_millis = millis();
  if (current_millis - lastControlTime >= controlPeriod) {
    Actuator.writeMicroseconds(u_k);
    lastControlTime = current_millis;

    Serial.print(u_k);
    Serial.print(" ");
    Serial.print(rpm);
    Serial.print(" ");
    Serial.print(current_pos);
    Serial.print(" ");
    Serial.print(micros());
    Serial.print("\n");
  }
}

void loop() {

  // check actuator limits
  current_pos = analogRead(pot_pin);
  if (current_pos >= POT_MAX || current_pos <= POT_MIN) {
    u_k = 0;
    LCD.DispStringAt("   ", 68, 20);
  }
  
  // check button presses
  if (button1_pin == HIGH) {
    u_k = u_k_min;
    LCD.DispStringAt("IN ", 68, 20);
  } else if (button2_pin == HIGH) {
    u_k = u_k_max;
    LCD.DispStringAt("OUT", 68, 20);
  } else if (button3_pin == HIGH) {
    LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_NO_BAC);
    LCD.DispStringAt("send nudes", 0, 0);
    LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
  } else if (button4_pin == HIGH) {
    LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_NO_BAC);
    LCD.DispStringAt("Taiwan #1 ", 0, 0);
    LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
  }
  
  // update display rpm at appropriate interval
  int current_time = millis();
  if (current_time - lastDisplayRefreshTime >= displayRefreshPeriod) {
    LCD.DispStringAt(rpm, 60, 40);
  }
}
