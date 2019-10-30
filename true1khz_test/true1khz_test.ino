unsigned long last_millis(0);
int count = 0;

void setup() {
  Serial.begin(115200);
  TCCR1A = 0x0;
  TCCR1B = 0x0;
  TCCR1B |= (1 << CS11) | (11 << CS10); // prescaler 64
  TCCR1B |= (1 << WGM12); // CTC mode
  TIMSK1 |= (1 << OCIE1A); // enable interrupt for OCR1A
  OCR1A = 249; // interrupt at 1ms
}

ISR(TIMER1_COMPA_vect) {
  count += 1;
}

void loop() {
  if (count == 1000) {
    Serial.println(millis());
    count = 0;
  }
}

// off by about 1/16000 s/s
