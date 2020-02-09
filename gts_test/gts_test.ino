volatile int tooth_count = 0;

void setup() {
  pinMode(2, INPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), inter, RISING);
}

void inter() {
  tooth_count++;
}

void loop() {
  Serial.println(tooth_count);
}
