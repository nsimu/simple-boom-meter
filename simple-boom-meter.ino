#include "LedControl.h"

int LEDS_DIN = 12;
int LEDS_CLK = 11;
int LEDS_CS = 10;
int NUM_DISPLAYS = 1;
LedControl LEDS = LedControl(LEDS_DIN, LEDS_CLK, LEDS_CS, NUM_DISPLAYS);

void setup() {
  analogReference(EXTERNAL);
  LEDS.shutdown(0, false);
  LEDS.setIntensity(0,4);
  LEDS.clearDisplay(0);
  Serial.begin(9600);
}

int MAX_SCALE = 9;
byte NUM_LEDS[9] = {
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
int MAX_PEAK_TO_PEAK = 400;

void loop(){
  int value = getPeakToPeak();
  Serial.println(value);
  int displayValue = map(value, 0, MAX_PEAK_TO_PEAK, 0, MAX_SCALE);
  for(int row = 0; row < 8; row++) {
    LEDS.setRow(0, row, NUM_LEDS[displayValue]);  
  }
}

unsigned int SAMPLE_WINDOW = 20; // ms
int MIKE_A0 = A0;

int getPeakToPeak() {
  unsigned long startMillis= millis();
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
