////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Actuator Position [inch] Control
//// Conceptualized and Coded by Iou-Sheng Chang, Brendon Anderson
//// Code tweaked by Iou-Sheng Chang
//// Code last editted: 06.27.2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Library
#include <Servo.h>

//// Servo object create
Servo linearActuator;

#define pinActuator 9
#define pinPot A0
#define pinHallEffect 2

//// Define RPM var and initialize
double rpm = 0;
double RPMCalc = 0;
double HighLow = 0;
double currentTime = 0;
double deltaTime = 0;
double previousTime = 0;

//// Define uk var
#define uk_max 2000
#define uk_min 1000
#define uk_stop 1460
#define uk_limit 100

//// Define Controller Timer interval [milliseconds]
#define TimedLoop 20
int lastControlTime(0);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //// Baud rate setup
  Serial.begin(9600);

  //// PinMode setup
  pinMode(pinPot, INPUT);
  pinMode(pinHallEffect,INPUT);
  
  //// Actuator setup && servo input set to not moving
  linearActuator.attach(pinActuator,uk_min,uk_max);
  linearActuator.writeMicroseconds(uk_stop);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  interrupts();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SIGNAL(TIMER0_COMPA_vect) {
  int currentMillis = millis();
  if (currentMillis - lastControlTime >= TimedLoop) {
    controlFunction();
    lastControlTime = currentMillis;
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Controller var setup & initialization
#define rk 2.68901617     // reference signal [in]
#define minPot 106        // pot reading when cvt is at min shift
#define maxPot 875        // pot reading when cvt is at max shift
#define engagePot 670     // value [670,680] when engages
#define minDist 0.8125    // Distance [in] related to minPot
#define deltaDist 4.9375  // Distance [in] related to maxPot - minPot
#define maxDist 5.75      // Distance [in] related to manPot
#define pk_min 150        // chosen value of pot reading for cvt min shift
#define pk_max 450        // chosen value of pot reading for cvt max shift
#define A 3/PI          // reference amplitude [in]
#define w 1.5*PI        // reference freq [rad/s]
#define COO 2           // reference center [in]
//double rk = 0;
double pk = 0;            // pot reading
double xk = 0;            // pk to xk [in]
double ek = 1;            // error [in]
double ek1 = 0;           // previous < error [in]
double ek2 = 0;           // << error [in]
//double ek3 = 0;           // <<< error [in]
double uk = 0;            // servo signal
double uk1 = 0;           // previous < servo signal
double uk2 = 0;           // << servo signal
//double uk3 = 0;           // <<< servo signal
double t;                 // time
bool reachedMin = false;  // cvt reached min shift
bool reachedMax = false;  // cvt reached max shift
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void controlFunction() {
  //// Compute desired reference signal (for sinusoid,chirp input)
//  t = micros()*pow(10,-6);  // convert time into [s]
//  rk = A*sin(w*t)+COO;      // reference value
  //// RPM Calculation
//  rpmCalc();
   
  //// Potentiometer reading
  pk = analogRead(pinPot);
  
  //// Compute pk to xk
  xk = abs((pk-minPot)*deltaDist/(maxPot-minPot)) + minDist;
  
  //// Compute error
  ek = rk - xk;
  
  //// Compute control input && constrain uk
//  uk = 0.8*uk1-0.07*uk2+126*ek-31.5*ek2;                // control input [-500,500]   OLD, DECENT CONTROLLER
//  uk = 0.8*uk1-0.07*uk2+216*ek-54*ek2;                  // control input [-500,500]   NEW, AGRESSIVE CONTROLLER
  uk = 0.25*uk1 + 0.125*uk2 + 1250*ek1 - 937.5*ek2;       // Controller Algorithm
  uk = constrain(uk,uk_min - uk_stop,uk_max - uk_stop);   // Constrain uk values to be [uk_min - uk_stop,uk_max + uk_stop]
  
  //// Store and update previous uk/ek values  
  //uk3 = uk2;
  uk2 = uk1;
  uk1 = uk;
  //ek3 = ek2;
  ek2 = ek1;
  ek1 = ek;

  //// Safety constrain
  if (pk <= pk_min) {                               // checks if cvt is at min shift
    reachedMin = true;                              // cvt has now reached min shift
    uk = uk_limit;
    linearActuator.writeMicroseconds(uk + uk_stop); // send - servo signal to shift actuator inward
  }
  if (pk >= pk_max) {                               // checks if cvt is at max shift
    reachedMax = true;                              // cvt has now reached max shift
    uk = -uk_limit;
    linearActuator.writeMicroseconds(uk + uk_stop); // send + servo signal to shift actuator outward
  }
  
  //// Sending servo signal
  if (!reachedMin && !reachedMax) {
    linearActuator.writeMicroseconds(uk + uk_stop);  // Send computed Servo Signal
  }

  //// Write data
//  writeData();
  
  //// Reset boolean of reachedMin && reachedMax
  reachedMin = false;
  reachedMax = false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //// Write data
  writeData();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// RPM Calculation
void rpmCalc(){
  if (analogRead(pinHallEffect) < 100) {
    if (HighLow == 1) {
      RPMCalc = 1;
    }
    HighLow = 0;
  }
  if (analogRead(pinHallEffect) > 1000) {
    HighLow = 1;
  }
  if (RPMCalc == 1) {
    deltaTime = (micros() - previousTime);    // Calculate change in time
    rpm = (60000000.0 / deltaTime);           // RPM is 60,000,000 microseconds in a minute (x2 because 2 rotations have been detected), divided by the change in time since last calculation
    RPMCalc = 0;
    previousTime = micros();
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Write Data
void writeData(){
  Serial.print(", ");
  Serial.print(uk);
  Serial.print(", ");
  Serial.println(pk);
//  String data = "";
//  data += String(uk);
//  data += ", ";
//  data += String(rpm);
//  data += ", ";
//  data += String(pk);
//  data += ", ";
//  data += String(reachedMin);
//  data += ", ";
//  data += String(reachedMax);
//  Serial.println(data);       // Print to Serial Monitor
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
