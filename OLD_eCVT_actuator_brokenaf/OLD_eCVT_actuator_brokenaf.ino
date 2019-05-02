////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// eCVT RPM Controller
//// Conceptualized and Coded by Iou-Sheng Chang, Brendon Anderson, Simon Rufer
//// Controller first successfully implemented on 06.27.2018 (PI controller)
//// Code tweaked by Brendon Anderson, Iou-Sheng Chang
//// Code last editted: 06.28.2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Library
#include <Servo.h>

//// Servo object create
Servo linearActuator;

double last_switch = 0;
double period = 16E6;
//double period1 = 2*period;
//double period2 = 3*period;
int uk;
//int count = 1;

//// Define uk var
#define uk_max 1525
#define uk_min 1400
#define uk_stop 1460

//// Define Yun pins
#define pinActuator A3    // Designating actuator to pin A3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int time_off = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //// Baud rate setup
  Serial.begin(9600);

  //// Actuator setup && servo input set to not moving
  linearActuator.attach(pinActuator, uk_min, uk_max);
  linearActuator.writeMicroseconds(uk_stop);

  time_off = micros();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //// wave
  if ((micros()-time_off - last_switch) >= period) {
    last_switch = micros();
    if (uk == uk_max) {
      uk = uk_min;
    } else {
      uk = uk_max;
    }
    linearActuator.writeMicroseconds(uk);
  }
//  uk = (uk_max - uk_min)/2 * sin(micros()*2*M_PI/period) + （uk_max + uk_min)/2;
  Serial.println(uk);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////