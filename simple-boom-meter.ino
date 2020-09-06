#include "LedControl.h"

#define LEDS_DIN 12
#define LEDS_CLK 11
#define LEDS_CS 10
#define NUM_DISPLAYS 1
LedControl LEDS = LedControl(LEDS_DIN, LEDS_CLK, LEDS_CS, NUM_DISPLAYS);

void setup() {
  analogReference(EXTERNAL);

  LEDS.shutdown(0, false);
  LEDS.setIntensity(0, 4);
  LEDS.clearDisplay(0);

  Serial.begin(9600);
}

# define MAX_SCALE 9
byte NUM_LEDS[MAX_SCALE] = {
  B00000000,
  B10000000,
  B11000000,
  B11100000,
  B11110000,
  B11111000,
  B11111100,
  B11111110,
  B11111111,
};
byte mode = 0; // 0 = PeakToPeak, 1 = RootMeanSquares
unsigned int maxInputsByMode[] = { 400, 75 };
int value = 0;

void loop() {
  if (mode == 0) {
    value = getPeakToPeak();
  } else if (mode == 1) {
    value = getRootMeanSquares();
  }
  Serial.println(value);
  int displayValue = map(value, 0, maxInputsByMode[mode], 0, MAX_SCALE);
  for(int row = 0; row < 8; row++) {
    LEDS.setRow(0, row, NUM_LEDS[displayValue]);  
  }
}

#define SAMPLE_WINDOW 20 // ms
#define MIKE_A0 A0

int getPeakToPeak() {
  unsigned long startMillis = millis();
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  while (millis() - startMillis < SAMPLE_WINDOW) {
    int sample = analogRead(MIKE_A0);
    if (sample > signalMax) {
      signalMax = sample;
    } else if (sample < signalMin) {
      signalMin = sample;
    }
  }
  return signalMax - signalMin;
}

#define BASELINE 283

int getRootMeanSquares() {
  unsigned long startMillis = millis();
  unsigned long sumOfSquares = 0;
  unsigned int numSamples = 0;
  while (millis() - startMillis < SAMPLE_WINDOW) {
    int sample = analogRead(MIKE_A0) - BASELINE;
    numSamples++;
    sumOfSquares += sq(sample);
  }
  int value = round(sqrt(sumOfSquares / numSamples));
  return value;
}
