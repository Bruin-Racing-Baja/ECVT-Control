#include <Servo.h>

#define NEUTRAL_SHIFT 1460
#define FULL_FORWARD_SHIFT  2000 //DOUBLE CHECK VALUE
#define FULL_REVERSE_SHIFT  1000

// ACTUATOR STUFF
int pot_read_in = A0;
int max_value = 480;
int min_value = 180;

const int ActuatorPin = 3;
int pwm_value(0);
const double w = 2;//define per test
const double w_max = 1.2;// rad/sec
const double A_max = 450;
const int A = A_max*w/w_max;;

Servo Actuator;
int timeOffset(0);

// RPM STUFF
//int rpm_reader = 2;
//int previous_Time = 0;
//int current_Time = 0;
//int RPM = 0;

void setup() 
{
Serial.begin(9600);
delay(50);
pinMode(pot_read_in, INPUT);
//attachInterrupt(digitalPinToInterrupt(rpm_reader), rpmCalc, CHANGE);//could do RISING
Actuator.attach(ActuatorPin);
timeOffset = millis();
}

void loop()
{
//Serial.println(RPM)
/*
if(pot_read_in >= max_value)
{
  //move back
Actuator.writeMicroseconds(FULL_REVERSE_SHIFT+200);
 // return;
}else if(pot_read_in <= min_value)
{
  //move forward
  Actuator.writeMicroseconds(FULL_FORWARD_SHIFT-200);
  //return;
}
*/
//move the actuator 
pwm_value = A*sin(w*(millis()-timeOffset)/1000) + NEUTRAL_SHIFT;
Serial.println(pwm_value);
Actuator.writeMicroseconds(pwm_value);
//Serial.println(RPM);


}
//void rpmCalc()
//{
//
//previous_Time = current_Time;
//current_Time = millis();
//RPM = 15000/(current_Time - previous_Time);//30000, 
//
//}

