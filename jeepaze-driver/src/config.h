// -- Bluetooth
#include <SoftwareSerial.h>
#define BT_RX 4
#define BT_TX 5
#define BT_BAUD 9600
SoftwareSerial BlueSerial(BT_RX, BT_TX);

// -- Buttons
#define NB_PIN 2
#define SB_PIN 3
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000; 

// -- Main
#include "routes.h"
int currentStation = 0;
String direction = "North";
bool interrupted = false;
int passengerLimit = 20;
int passengersLeft = passengerLimit;
bool stationChanged = false;


// -- LCD Settings
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);