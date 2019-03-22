////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Actuator Position [inch] Control
//// Conceptualized and Coded by Iou-Sheng Chang, Brendon Anderson
//// Code tweaked by Iou-Sheng Chang
//// Code last editted: 06.27.2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Library
#include <Servo.h>
#include <ADC.h>

//// Servo object create
Servo linearActuator;

//// ADC object (for potentiometer reading)
ADC *adc = new ADC();

//// Timer Object create
  //Create an IntervalTimer object
  //Normally IntervalTimer objects should be created as global variables
IntervalTimer myTimer;

//// Define Teensy 3.5 pins
#define pinActuator A3    // Designating actuator to pin A3
#define pinPot A5         // Designating actuator potentiometer to pin A5
#define pinHallEffect A9  // Designating Hall Effect sensor to pin A9

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

//// Define Controller Timer interval [microseconds]
#define TimerInterval 4000
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

  //// Timer setup for controlFunction
    // The interval is specified in microseconds, may be an integer or floating point number
    // This function returns true if successful, false is returned if all hardware resources are busy, used by other IntervalTimer objects
  myTimer.begin(controlFunction,TimerInterval);
  interrupts();

  //// ADC setup
    // ADC0 //
  adc -> setAveraging(64);                                                  // set number of averages
  adc -> setResolution(10);                                                 // set bits of resolution
  adc -> setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);         // change the conversion speed
  adc -> setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);             // change the sampling speed
    // ADC1 //
  #if ADC_NUM_ADCS > 1
  adc -> setAveraging(64, ADC_1);                                           // set number of averages
  adc -> setResolution(10, ADC_1);                                          // set bits of resolution
  adc -> setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_1);  // change the conversion speed
  adc -> setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_1);      // change the sampling speed
  #endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Controller var setup & initialization
//#define rk 2.68901617     // reference signal [in]
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
double rk = 0;
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
  t = micros()*pow(10,-6);  // convert time into [s]
  rk = A*sin(w*t)+COO;      // reference value
  //// RPM Calculation
//  rpmCalc();
   
  //// Potentiometer reading
  pk = adc -> adc0 -> analogRead(pinPot);
  
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
  Serial.print(rk);
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
