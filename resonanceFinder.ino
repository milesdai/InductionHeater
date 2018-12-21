float freq = 100; // Frequency in kHz
bool buttonFlag = false;
long cusum = 0;
int count = 0;

#define VOLTAGE_SENSE_PIN 14
#define PWM_PIN 5
#define BTN_PIN 6

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP); // Btn pin
  pinMode(VOLTAGE_SENSE_PIN, INPUT); // analog read pin
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), btnISR, CHANGE);
  analogWriteFrequency(PWM_PIN, freq * 1000);
  Serial.begin(19200);
}

void loop() {
  cli(); // disable interrupts
  if(buttonFlag){
    findResonance();
    buttonFlag = false;
  }
  sei(); // enable interrupts
  analogWrite(PWM_PIN, 127); // 50% duty cycle
}

void findResonance() {
  int cMax = 0; // Current max response
  int bestF = 0;
  // Test from 50-150 kHz in increments of 10kHz
  for(int i = 50; i < 150; i+=10){
    analogWriteFrequency(PWM_PIN, i * 1000);
    delay(20);
    int response = sample();
    Serial.println(response);
    if(response >= cMax) {
      cMax = response;
    } else {
      bestF = i - 10;
      analogWriteFrequency(PWM_PIN, bestF * 1000);
      break;
    }
  }

  // Test best range from above in 1 kHz steps
  cMax = 0;
  for(int j = bestF; j < (bestF + 10); j+= 1) {
    analogWriteFrequency(PWM_PIN, j * 1000);
    delay(20);
    int response = sample();
    Serial.println(response);
    if(response >= cMax) {
      cMax = response;
    } else {
      bestF = j - 1;
      freq = bestF;
      analogWriteFrequency(PWM_PIN, freq * 1000);
      break;
    }
  }
}

long sample() {
  analogWrite(PWM_PIN, 127);
  long cusum = 0;
  for(int i = 0; i < 5000; i++) {
    cusum += analogRead(VOLTAGE_SENSE_PIN);
  }
  return cusum / 5000;
}

void btnISR() {
  buttonFlag = true;
}

