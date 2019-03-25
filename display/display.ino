// display setup
#include <Wire.h>
#include <I2C_LCD.h>
I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51;
extern GUI_Bitmap_t bmBruinRacing;
extern GUI_Bitmap_t bmBearHead;
const int refreshPeriod = 100; // display refresh time [ms]
int lastRefreshTime(0);

// push buttons setup
const int pinButton1 = 13;
const int pinButton2 = 12;
int mode = 0;
char mode_names[2][7] = {"POWER ", "TORQUE"};

void setup() {
  // configure button pins
  pinMode(pinButton1, INPUT_PULLUP);
  pinMode(pinButton2, INPUT_PULLUP);
  
  // init I2C interface
  Wire.begin();
  // configure LCD for boot logo OFF, backlight ON, bitmap display mode
  LCD.WorkingModeConf(OFF, ON, WM_BitmapMode);

  // clear screen and display sweaty bruin
  LCD.CleanAll(WHITE);
  LCD.DrawScreenAreaAt(&bmBruinRacing, 0, 1);
  delay(3000);
  LCD.CleanAll(WHITE);

  // configure LCD for character display mode , 8x16 font, auto-newline, black text with background
  LCD.DrawScreenAreaAt(&bmBearHead, 0, 0);
  LCD.WorkingModeConf(OFF, ON, WM_CharMode);
  LCD.FontModeConf(Font_6x8, FM_MNL_AAA, BLACK_NO_BAC);
  LCD.DispStringAt("MODE:", 68, 5);
  LCD.DispStringAt("RPM:", 68, 35);
  LCD.FontModeConf(Font_8x16_2, FM_MNL_AAA, BLACK_BAC);
}

void loop() {
  // check for button presses
  if (digitalRead(pinButton1) == LOW) {
    mode = 0;
  } else if (digitalRead(pinButton2) == LOW) {
    mode = 1;
  }

  // update screen at appropriate interval
  int current_time = millis();
  if (current_time - lastRefreshTime >= refreshPeriod) {
    update_display();
    lastRefreshTime = current_time;
  }
}

void update_display() {
  LCD.DispStringAt(mode_names[mode], 78, 15);
  LCD.DispStringAt("3600", 78, 45);
}

