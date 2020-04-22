/* m19_eg_sid.ino
 *
 * This script runs various system identification tests to build a plant model
 * for the powertrain. Select the input signal by changing (*signal_function)
 * and setting the appropriate signal parameters. The tests are meant to run on
 * a jackstand with a geartooth sensored engine and hall sensored gearbox.
 * 
 * On Model 19 the hardware returns so much noise that the data becomes
 * unusable. That is the reason for all of the debugging files in this directory.
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 * created: 19 February 2020
 */

#include <PCF8593.h>
#include <Servo.h>

// PWM constants
#define PW_STOP 1515
#define PW_MIN 1000
#define PW_MAX 2000
#define U_K_ABS_MIN PW_MIN - PW_STOP
#define U_K_ABS_MAX PW_MAX - PW_STOP

// actuator
Servo Actuator;
const byte actuator_pin = 10;
#define POT_MIN 166
#define POT_MAX 254
#define POT_ENGAGE 245
int pot_lim_out = POT_MAX;
int pot_lim_in = POT_MIN;
int u_k_min = U_K_ABS_MIN; // [dpwm]
int u_k_max = U_K_ABS_MAX; // [dpwm]
const byte pot_pin = A1;
int pos_now(0);

// controller
const unsigned int control_period = 20e3;
unsigned long last_control_time(0);
const int Kp = 25;
int pos_ref(0);
int pos_error(0);
int u_k(0);
int u_k_final(0);
int (*signal_function)(unsigned long) = &const_ref;

// sine signal
const int sine_amp = (POT_ENGAGE - POT_MIN)/2 - 15; // [pot counts]
const int sine_off = (POT_ENGAGE + POT_MIN)/2; // [pot counts]
const double sine_freq = .2; // [Hz]

// triangle signal
const int triangle_slope = 40; // [pot counts/sec]
const int triangle_max = POT_ENGAGE - 2; // [pot counts]
const int triangle_min = POT_MIN + 2; // [pot counts]
const long triangle_per = 2*(triangle_max - triangle_min)*1000000L/triangle_slope; // [ms]

// logarithmic chirp signal
const double chirp_w1 = 2*PI*.01; // [2pi*Hz]
const double chirp_w2 = 2*PI*10; // [2pi*Hz]
const int chirp_per = 20; // [s]
double chirp_alpha = 1./chirp_per*log(chirp_w2/chirp_w1);

// reference pot signal
byte ref_pot_pin = A2;

// constant signal
int const_pos = POT_MAX;

// engine sensor (PCF8593 interface)
byte csr_val = 0x00 | 1<<5 | 1<<2; // set control and status register to event counter mode with alarm control enabled
long eg_last_count(0);
int eg_rpm(0);
double eg_pulseToRpm = 35.714285714285715;

// gearbox sensor
const byte gb_pin = 3;
bool gb_state(LOW);
unsigned long gb_last_trigger(0);
int gb_rpm(0);
double gb_conversion_factor = 11020408.163265307; // [(rot*us)/(count*min)]

void setup() {

  // arduino is a good boy
  bool good_boy = true;

  // open serial connection
  Serial.begin(256000);

  // join i2c bus
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  Wire.begin();

  // setup actuator
  Actuator.attach(actuator_pin, PW_MIN, PW_MAX);
  Actuator.writeMicroseconds(PW_STOP);
  pinMode(pot_pin, INPUT);
  pos_now = analogRead(pot_pin);

  // setup engine sensor
  set_csr(csr_val);
  reset_counters();

  // setup gearbox sensor
  pinMode(gb_pin, INPUT);

  // setup timer interrupt
  // OCR0A = 0xFF;
  // TIMSK0 |= _BV(OCIE0A);
}

// SIGNAL(TIMER0_COMPA_vect) {
// 
// }

void control_function(unsigned long t) {

  // actuator position controller
  pos_ref = signal_function(t);
  pos_now = analogRead(pot_pin);
  pos_error = pos_ref - pos_now;
  u_k = Kp*pos_error;
  u_k_final = constrain(u_k, u_k_min, u_k_max);
  Actuator.writeMicroseconds(u_k_final + PW_STOP);

  // engine rpm
  long eg_count = get_count();
  eg_rpm = (eg_count - eg_last_count)*eg_pulseToRpm;
  // eg_count = eg_rpm;

  // print data
  Serial.print(pos_ref);
  Serial.print(",");
  Serial.print(pos_now);
  Serial.print(",");
  Serial.print(eg_rpm);
  Serial.print(",");
  Serial.print(gb_rpm);
  // Serial.print(",");
  // Serial.print(t);
  Serial.print("\n");

  eg_last_count = eg_count;
}

int sine(unsigned long t) {
  return sine_off + sine_amp*sin(2*PI*sine_freq*t/1.e6);
}

int triangle(unsigned long t) {
  t = t % triangle_per;
  bool rising = (t < triangle_per/2);
  return (triangle_min + triangle_slope*t/1.e6)*rising + (triangle_max - triangle_slope*(t-triangle_per/2)/1.e6)*(1-rising);
}

int chirp(unsigned long t) {
  t = t % (1000000*chirp_per);
  double f = chirp_w1/chirp_alpha*(exp(chirp_alpha*t/1.e6) - 1);
  return sine_off + sine_amp*sin(f);
}

int pot_ref(unsigned long t) {
  int pot_reading = analogRead(ref_pot_pin);
  return map(pot_reading, 0, 1023, POT_MIN, POT_ENGAGE);
}

int const_ref(unsigned long t) {
  return const_pos;
}

void loop() {

  unsigned long current_micros = micros();

  // // check gearbox rpm
  bool gb_reading = digitalRead(gb_pin);
  if (gb_reading == HIGH) {
    gb_state = HIGH;
  } else if (gb_reading == LOW && gb_state == HIGH) {
    gb_rpm = gb_conversion_factor/(current_micros - gb_last_trigger);
    gb_last_trigger = current_micros;
    gb_state = LOW;
  }

  if (current_micros - last_control_time >= control_period) {
      control_function(current_micros);
      last_control_time = current_micros;
    }
}
