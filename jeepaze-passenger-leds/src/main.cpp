#include <Arduino.h>

const int commonPin = 2;
const int LEDPins[] = {
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

// const int LEDPins[] = {4,5,6,7,8,9,10,11,12,13};

void initLEDS()
{
  for (int i = 0; i < sizeof(LEDPins) / sizeof(int); i++)
  {
    pinMode(LEDPins[i], OUTPUT);
    digitalWrite(LEDPins[i], LOW);
  }
}

void testLEDS()
{
  for (int i = 0; i < sizeof(LEDPins) / sizeof(int); i++)
  {
    pinMode(LEDPins[i], OUTPUT);
    digitalWrite(LEDPins[i], HIGH);
    delay(25);
  }
}

void turnOnLED(int pin)
{
  initLEDS();
  Serial.print("Turning on: " + String(pin));
  pinMode(LEDPins[pin], OUTPUT);
  digitalWrite(LEDPins[pin], HIGH);
}

// void testLEDS()
// {
//   for (int i = 0; i < sizeof(LEDPins) / sizeof(int); i++)
//   {
//     digitalWrite(LEDPins[i], HIGH);
//   }
// }

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Hi!");
  initLEDS();
  testLEDS();
  delay(500);
  initLEDS();
}

void loop()
{
  if (Serial1.available() > 0)
  {
    char inputBuffer[16];
    int val;
    Serial1.readBytes(inputBuffer, sizeof(inputBuffer));
    val = atoi(inputBuffer);
    memset(inputBuffer, 0, sizeof(inputBuffer));
    Serial.println(val);

    // String buffer= Serial1.readString();
    // Serial.println(buffer);
    // char c = Serial1.read();
    // Serial.println(c);
    // int inByte = Serial1.parseInt();
    // Serial.println(inByte);
    // int val = Serial1.parseInt();
    // Serial.println(val);
    if (val >= 0)
    {
      turnOnLED(val);      
    }
    delay(1000);
  }
}