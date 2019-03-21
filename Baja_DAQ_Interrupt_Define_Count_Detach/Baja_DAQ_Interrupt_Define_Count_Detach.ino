//Drivetrain Dynamometer (1/1/18)
//Conceptualized by Jong man Park
//Code by He Kai Lim: 01/01/18; Code by Iou Sheng Chang: 01/07/18
//Code tweaked by Iou Sheng Chang: 01/09/18
  //RPM Calculation change from Duration to Count 01/09/18
  //DetachInterrupt added 01/10/18
//Code last editted: 01/30/18 15:00

////Microcontroller pins setup
  //Hall Effect RPM: digital connection
  //Pressure: Analog connection
#define pinHallEffect 0

////Flywheel #fins setup
#define numMag 4 //Number of sensor magnets on the flywheel

////Sampling period setup
#define count_samplingInterval 40

unsigned int count;

unsigned long lasttime;
unsigned long duration;

////Define variable type: Output
unsigned int rpm;

////Input_HallEffect(pin2) Interrupt loop
void inputRpmInterrupt(){
  count += 1;
}

void setup() {
  ////pinMode setup
    //pinMode for HallEffect is not required
  pinMode(pinHallEffect, INPUT);    //Sets up Input_HallEffect(interrupt pin 0, pin D2) as the input shaft rpm

  ////Initialize all variables to 0
//  count = 0;  //HallEffect initialization
//  rpm = 0;
//
//  ////Logged in lasttime values from micros()
//  lasttime = micros();

  ////attachInterrupt
    //Interrupt Mode: FALLING (trigger the interrupt whenever the pin goes from HIGH to LOW)
      //FALLING is for unipolar HallEffect Sensor
    //Interrupt Mode: CHANGE (trigger the interrupt whenever the pin changes value)
      //CHANGE is for bipolar HallEffect Sensor
      //NOTE: the magnets should be set up as N/S/N/S
//  attachInterrupt(digitalPinToInterrupt(pinHallEffect), inputRpmInterrupt, CHANGE);

  //Baud Rate Setup
  Serial.begin(9600);  // Specify connection type between Arduino and compiler
}

void loop() {
  ////Duration that has passes b/w each RPM calculation
    //Duration is in microsecond
//  duration = micros() - lasttime;
//  
//  //HallEffect
//  if(count >= count_samplingInterval)
//  {
//    //detachInterrupt
//    detachInterrupt(pinHallEffect);
//    
//    //RPM Calculation
//    rpm = 60000000*count/numMag/duration; //HallEffect RPM
//
//    //Update count to 0
//    count = 0;
//
//    //attachInterrupt
//    attachInterrupt(digitalPinToInterrupt(pinHallEffect), inputRpmInterrupt, CHANGE);
//
//    //Update new lasttime
//    lasttime = micros();
//  }
//
//  ////Serial Monitor
//  Serial.println(rpm);
  Serial.println(analogRead(pinHallEffect));
  Serial.flush();
}
