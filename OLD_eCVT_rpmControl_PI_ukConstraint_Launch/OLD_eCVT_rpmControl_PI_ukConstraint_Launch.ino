////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// eCVT RPM Controller
//// Conceptualized and Coded by Iou-Sheng Chang, Brendon Anderson, Simon Rufer
//// Controller first successfully implemented on 06.27.2018 (PI controller)
//// Code tweaked by Iou-Sheng Chang, Brendon Anderson
//// Code last editted: 06.30.2018
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
  // Create an IntervalTimer object
  // Normally IntervalTimer objects should be created as global variables
IntervalTimer myTimer;

//// Define Teensy 3.5 pins
#define pinActuator A8    // Designating actuator to pin A3
#define pinPot A3         // Designating actuator potentiometer to pin A5
#define pinHallEffect A6  // Designating Hall Effect sensor to pin A9

//// Define RPM var and initialize
double rpm = 0;
double rpm_prev = 0;
double RPMCalc = 0;
double HighLow = 0;
double currentTime = 0;
double deltaTime = 0;
double previousTime = 0;

//// Define uk var
#define uk_max 2000
#define uk_min 1000
#define uk_stop 1460
#define duk_limit 100

//// Define Controller Timer interval [microseconds]
#define TimerInterval 50
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
#define rk 3400               // reference signal [rpm]
#define rpm_engage 2800       // speed to start engaging
#define minPot 106            // pot reading when cvt is at min shift
#define maxPot 875            // pot reading when cvt is at max shift
#define engagePot 650         // value [670,680] when engages
#define pk_min 750            // chosen value of pot reading for cvt min shift
#define pk_max 110            // chosen value of pot reading for cvt max shift
double pk_var = pk_min;       // pk_var for different rpm pl_min setup
double pk = 0;                // pot reading
double ek = 1;                // error [rpm]
double ek1 = 0;               // previous < error [rpm]
double ek2 = 0;               // << error [rpm]
double uk = 1;                // servo signal
double uk1 = 0;               // previous < servo signal
double uk2 = 0;               // << servo signal
double duk = 1;
double duk1 = 0;
double t;                     // time [s]
bool reachedMin = false;      // cvt reached min shift
bool reachedMax = false;      // cvt reached max shift

// PI
#define k 27.309502622699998  // PI controller GAIN
#define z 0.999889956055168   // PI controller ZERO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void controlFunction() {
  //// RPM Calculation
  rpmCalc();

  //// Potentiometer reading
  pk = adc -> adc0 -> analogRead(pinPot);
  
  //// Compute error
  ek = rk - rpm;
  
  //// Compute control input && constrain uk
  duk = duk1 + k*ek - (k*z)*ek1;                      // controller Algorithm for duk and ek
  uk = duk + uk_stop;                                 // input to actuator
  uk = constrain(uk,uk_min,uk_max);                   // Constrain uk values to be [uk_min,uk_max]

  //// Constrain
  if (rpm >= rpm_engage) {
    pk_var = engagePot;                               // 650
  }
  else {
    pk_var = pk_min;                                  // 750
  }
  
  //// Safety constrain
  if (pk >= pk_min) {                                 // checks if cvt is at min shift
    reachedMin = true;                                // cvt has now reached min shift
    duk = -duk_limit;
    linearActuator.writeMicroseconds(duk + uk_stop);  // send - servo signal to shift actuator inward
  }
  else if (pk >= pk_var) {
    reachedMin = true;                                // cvt has now reached min shift (rpm >= rpm_engage)
    duk = uk_min - uk_stop;
    linearActuator.writeMicroseconds(duk + uk_stop);  // send uk_min servo signal to shift actuator inward to be engage
  }
  if (pk <= pk_max) {                                 // checks if cvt is at max shift
    reachedMax = true;                                // cvt has now reached max shift
    duk = duk_limit;
    linearActuator.writeMicroseconds(duk + uk_stop);  // send + servo signal to shift actuator outward
  }
  
  //// Sending servo signal
  if (!reachedMin && !reachedMax) {
    linearActuator.writeMicroseconds(uk);             // Send computed Servo Signal
  }

  //// Write data
  writeData();

  //// Store and update previous uk/ek values  
  duk1 = duk;
  ek1 = ek;
  rpm_prev = rpm;
  
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
    deltaTime = (micros() - previousTime);
    rpm = (60000000.0 / deltaTime);
    RPMCalc = 0;
    previousTime = micros();
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Write Data
void writeData(){
  String data = "";
  data += String(duk);
  data += ", ";
  data += String(uk);
  data += ", ";
  data += String(rpm);
  data += ", ";
  data += String(pk);
  data += ", ";
  data += String(reachedMin);
  data += ", ";
  data += String(reachedMax);
  Serial.println(data);       // Print to Serial Monitor
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
