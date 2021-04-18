#include <Arduino.h>

const int commonPin = 2;
const int buttonPins[] = {
    22, // N0 = 0
    23, // N1 = 1
    24, // N2 = 2
    25, // N3 = 3
    26, // N4 = 4
    27, // N5 = 5
    28, // N6 = 6
    29, // N7 = 7
    30, // N8 = 8
    31, // N9 = 9
    32, // S0 = 11
    33, // S1 = 12
    34, // S2 = 13
    35, // S3 = 13
    36, // S4 = 13
    37, // S5 = 13
    38, // S6 = 13
    39, // S7 = 13
    40, // S8 = 13
    41, // S9 = 13
};

// const int buttonPins[] = {4,5,6,7,8,9,10,11,12,13};


unsigned long lastFire = 0;

void configureCommon() {
  pinMode(commonPin, INPUT_PULLUP);

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++) {
    pinMode(buttonPins[i], OUTPUT);
    digitalWrite(buttonPins[i], LOW);
  }
}

void configureDistinct()
{
  pinMode(commonPin, OUTPUT);
  digitalWrite(commonPin, LOW);

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void press(int button)
{ // Our handler
  Serial.print(button);
  Serial1.print(button);
}

void pressInterrupt() { // ISR
  if (millis() - lastFire < 200) { // Debounce
    return;
  }
  lastFire = millis();

  configureDistinct(); // Setup pins for testing individual buttons

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++) { // Test each button for press
    if (!digitalRead(buttonPins[i])) {
      press(i);
    }
  }

  configureCommon(); // Return to original state
}

void setup()
{
  Serial.begin(9600);  
  Serial1.begin(9600);  
  configureCommon();
  attachInterrupt(digitalPinToInterrupt(commonPin), pressInterrupt, FALLING);
}

void loop()
{
}