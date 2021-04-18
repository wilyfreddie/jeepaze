#include <Arduino.h>

// -- Bluetooth Settings - Driver
#include <SoftwareSerial.h>
#define BT_BAUD 9600
#define BT_RX 8
#define BT_TX 9

// HardwareSerial BTSerial = Serial1;
SoftwareSerial BTSerial(BT_RX, BT_TX);


// -- Bluetooth Settings - Driver
#include <SoftwareSerial.h>
#define BT_LED_BAUD 9600
#define BT_LED_RX 16
#define BT_LED_TX 17

HardwareSerial LEDSerial = Serial2;
// SoftwareSerial LEDSerial(BT_RX, BT_TX);

// -- LCD Settings
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);

// -- GPS Settings
#include <TinyGPS++.h>
#define GPS_RX 14
#define GPS_TX 15
#define GPS_BAUD 9600

TinyGPSPlus gps;
HardwareSerial GPSSerial = Serial3;
// SoftwareSerial GPSSerial(GPS_RX, GPS_TX);
float curr_lat;
float curr_lng;

int gpsTempCount = 0;

// -- RFID Settings
#include <SPI.h>
#include <MFRC522.h>

#define RFID_RST 5
#define RFID_SDA 53
#define RFID_MOSI 51
#define RFID_MISO 50
#define RFID_SCK 52

MFRC522 mfrc522(RFID_SDA, RFID_RST);
MFRC522::MIFARE_Key key;

const byte block = 4;
const byte balanceBlock = 4;
const byte infoBlock = 5;
const byte trailerBlock = 7;

byte sector = 1;
byte blockAddr = 4;
byte dataBlock[] = {
    0xff, 0xff, 0xff, 0xff, //  1,  2,   3,  4,
    0xff, 0xff, 0xff, 0xff, //  5,  6,   7,  8,
    0xff, 0xff, 0xff, 0xff, //  9, 10, 255, 11,
    0xff, 0xff, 0xff, 0xff  // 12, 13, 14, 15
};

// -- Keypad Settings
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {28, 26, 24, 22}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {29, 27, 25, 23}; //connect to the column pinouts of the kpd

// Button Array
#define BUTT_TX 12
#define BUTT_RX 13
#define BUTT_BAUD 9600
SoftwareSerial BUTTSerial(BUTT_RX, BUTT_TX);

// Keypad Pisn 8,7,6,5 -> 28,26,24,22
// Keypad pins 4,3,2,1 -> 29,27,25,23

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Reload Button
#define reloadButton 2 // Must be an ISR pin!
unsigned long lastDetectedReloadButton = 0;

// -- Global Settings
#include "routes.h"
#include <ArduinoJson.h>
#define FAREBASE 10
#define FAREMULTIPLIER 8

int currentStation = 0;
int mode = 0; // 0 - Normal; 1 - Reload
short maxReloadAmount = 4000;





