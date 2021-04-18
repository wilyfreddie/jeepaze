#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"

void initBluetooth()
{
  BlueSerial.begin(BT_BAUD);
}

void sendDirectionNorth(int SID)
{

  Serial.flush();
  StaticJsonDocument<100> doc;
  doc["action"] = "direction";
  doc["direction"] = "north";
  doc["sid"] = SID;
  Serial.println(doc.isNull());
  serializeJson(doc, BlueSerial);
  BlueSerial.flush();
  serializeJson(doc, Serial);
}

void sendDirectionSouth(int SID)
{

  Serial.flush();
  StaticJsonDocument<100> doc;
  doc["action"] = "direction";
  doc["direction"] = "south";
  doc["sid"] = SID;
  Serial.println(doc.isNull());
  serializeJson(doc, BlueSerial);
  BlueSerial.flush();
  serializeJson(doc, Serial);
}

void NB_ISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    currentStation = 0;
    direction = "north";
    interrupted = true;
    lastDebounceTime = millis();
  }
}

void SB_ISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    currentStation = SOUTHBOUND_NUM;
    direction = "south";
    interrupted = true;
    lastDebounceTime = millis();
  }
}

void initButtons()
{
  pinMode(NB_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NB_PIN), NB_ISR, FALLING);
  pinMode(SB_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SB_PIN), SB_ISR, FALLING);
}

void DisplayDirection()
{
  if (currentStation <= 9)
  {
    lcd.setCursor(14, 0);
    lcd.print("NORTH");
  }
  else
  {
    lcd.setCursor(14, 0);
    lcd.print("SOUTH");
  }
}

void DisplayToLCDNextStation()
{
  bool flag = true;
  for (int i = currentStation; i < TOTALSTATIONS; i++)
  {
    if (route[i].passengers > 0)
    {
      flag = false;
      lcd.setCursor(0, 0);
      lcd.print(route[i].name);
      lcd.setCursor(0, 1);
      lcd.print("Pass: " + String(route[i].passengers));
      break;
    }
  }
  if (flag == true)
  {
    lcd.clear();
  }
}

void DisplayAvailableSeats()
{
  lcd.setCursor(0, 3);
  lcd.print("Avail: " + String(passengersLeft));
  stationChanged = false;
}

void PrintDisplay(String text)
{
  lcd.clear(); // go home(); // go home
  lcd.home();
  lcd.noBlink();
  lcd.print(text);
}

void InitDisplay()
{
  Serial.println("Initializing LCD...");

  lcd.begin(20, 4);
  lcd.setBacklight(255);
  lcd.noBlink();
  lcd.clear(); // go home
  PrintDisplay("Jeepaze is loading...");
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  PrintDisplay("Setting Up Display.");
  InitDisplay();
  Serial.println("Setting up Bluetooth.");
  PrintDisplay("Setting up Bluetooth.");
  initBluetooth();
  PrintDisplay("Setting Up Buttons.");
  Serial.println("Setting up Buttons.");
  initButtons();
  PrintDisplay("Jeepaze");
  lcd.clear();
  DisplayToLCDNextStation();
}

void loop()
{
  if (interrupted == true)
  {
    Serial.println("Interrupted");
    Serial.println(direction);
    Serial.println(currentStation);
    if (direction == "north")
      sendDirectionNorth(currentStation);
    if (direction == "south")
      sendDirectionSouth(currentStation);
    Serial.println("End");
    interrupted = false;
  }

  if (BlueSerial.available() > 0)
  {
    // Serial.write(BlueSerial.read());
    String data = BlueSerial.readString();
    Serial.print(data);
    DynamicJsonDocument doc(100);
    DeserializationError error = deserializeJson(doc, data);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (doc.containsKey("action"))
    {
      if (doc["action"] == "passenger")
      {
        route[(int)doc["sid"]].passengers = route[(int)doc["sid"]].passengers + (int)doc["qty"];
        passengersLeft = passengersLeft - (int)doc["qty"];
        DisplayToLCDNextStation();
      }
      else if (doc["action"] == "station")
      {
        if (currentStation != doc["sid"])
        {
          stationChanged = true;
          currentStation = doc["sid"];
          passengersLeft = passengersLeft + route[currentStation].passengers;
          route[currentStation].passengers = 0;
          if (passengersLeft >= passengerLimit)
          {
            passengersLeft = passengerLimit;
          };
        }
        else
        {
          stationChanged = false;
        }
      }
    }
  }

  // Display to LCD Info
  if (stationChanged == true)
  {
    DisplayToLCDNextStation();
    stationChanged = false;
  }
  DisplayAvailableSeats();
  DisplayDirection();
}