/* true1khz_test.ino
 *
 * reconfigures the ATMEGA328P-PU internal timer1 to interrupt closer to 1kHz
 *
 * author: Tyler McCown (tylermccown@engineering.ucla.edu)
 */

unsigned long last_millis(0);
int count = 0;

void setup() {
  Serial.begin(9600);
  TCCR2A = 0x0;
  TCCR2A |= (1 << WGM21); // CTC mode on OCR2A
  TCCR2B = 0x0;
  TCCR2B |= (1 << CS22); // prescaler 64
  TIMSK2 |= (1 << OCIE2A); // enable OCR2A interrupt
  OCR2A = 249; // interrupt at 1.000 ms
}

ISR(TIMER2_COMPA_vect) {
  count += 1;
}

void loop() {
  if (count >= 1000) {
    Serial.println(millis());
    count = 0;
  }
}

// off by about 1/16000 s/s
