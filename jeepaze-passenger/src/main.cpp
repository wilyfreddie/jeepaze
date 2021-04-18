#include "config.h"

// Display Methods
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
  // pinMode(CONTRAST_PIN, OUTPUT);
  // pinMode(BACKLIGHT_PIN, OUTPUT);

  // digitalWrite(BACKLIGHT_PIN, HIGH);
  // analogWrite(CONTRAST_PIN, CONTRAST);

  lcd.begin(20, 4);
  // lcd.backlight();
  lcd.setBacklight(255);
  lcd.noBlink();
  lcd.clear(); // go home
  PrintDisplay("Jeepaze is loading...");
}

void updateBluetooth()
{
  // delay(500);
  // while (Serial.available())
  // {
  //   BTSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  // }
  while (BTSerial.available())
  {
    Serial.write(BTSerial.read()); //Forward what Software Serial received to Serial Port
  }
}

void InitBluetooth()
{
  Serial.println("Initializing bluetooth...");
  PrintDisplay("Initializing bluetooth...");
  BTSerial.begin(BT_BAUD);
  LEDSerial.begin(BT_LED_BAUD);
}

// Keypad Methods
void displayAmountKeypad(String str)
{
  lcd.setCursor(0, 2);
  lcd.print(str);
}

short getAmountFromKeypad()
{
  bool done = false;
  int amount = 0;
  String inputString = "";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter amount:");
  Serial.println("Enter desired amount: ");

  while (done == false)
  {
    char key = keypad.getKey();
    if (key)
    {
      if (key >= '0' && key <= '9')
      {
        inputString += key;
      }
      else if (key == 'D')
      {
        if (inputString.length() > 0)
        {
          amount = inputString.toInt();
          inputString = "";
          done = true;
        }
      }
      else if (key == '*')
      {
        inputString = "";
        displayAmountKeypad("                ");
      }
      displayAmountKeypad(inputString);
      Serial.println(inputString);
    }
  }
  Serial.println("Amount: " + String(amount));
  return (short)amount;
}

// GPS Methods
void InitGPS()
{
  Serial.println("Initializing GPS...");
  PrintDisplay("Initializing GPS...");
  GPSSerial.begin(GPS_BAUD);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (GPSSerial.available())
      gps.encode(GPSSerial.read());
  } while (millis() - start < ms);
}

void SetCurrentStationTest()
{
  gpsTempCount++;

  if (!(gpsTempCount % 5))
  {
    currentStation++;
  }
  if (gpsTempCount > 100)
  {
    gpsTempCount = 0;
    currentStation = 0;
  }

  Serial.println("currentStation: " + String(currentStation));

  // Send to Driver Unit
  DynamicJsonDocument doc(1024);

  doc["action"] = "station";
  doc["sid"] = currentStation;
  String buffer;
  // serializeJson(doc, buffer);
  serializeJson(doc, BTSerial);
  delay(2000);

  // Send to LED Array
  LEDSerial.print(currentStation);
}

void SetCurrentStation()
{

  int ind = 0;

  unsigned long min =
      (unsigned long)TinyGPSPlus::distanceBetween(
          curr_lat,
          curr_lng,
          route[0].lat,
          route[0].lng) /
      1000;

  for (int i = 1; i < TOTALSTATIONS; i++)
  {
    unsigned long distance =
        (unsigned long)TinyGPSPlus::distanceBetween(
            curr_lat,
            curr_lng,
            route[i].lat,
            route[i].lng) /
        1000;
    if (distance < min)
    {
      min = distance;
      ind = i;
    }
  }

  currentStation = ind;

  Serial.println("currentStation: " + String(currentStation));

  // Send to Driver Unit
  DynamicJsonDocument doc(1024);

  doc["action"] = "station";
  doc["sid"] = currentStation;
  String buffer;
  serializeJson(doc, BTSerial);
  delay(2000);

  // Send to LED Array
  LEDSerial.print(currentStation);
}

void UpdateGPS()
{
  smartDelay(1500);
  curr_lat = gps.location.lat();
  curr_lng = gps.location.lng();
  Serial.println(curr_lat);
  Serial.println(curr_lng);
  SetCurrentStation();
}

void InitButtonArray()
{
  Serial.println("Initializing Button Array...");
  PrintDisplay("Initializing Button Array...");
  BUTTSerial.begin(BUTT_BAUD);
}

// RFID Methods
void InitRFID()
{
  Serial.println("Initializing RFID...");
  PrintDisplay("Initializing RFID...");
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();

  // Prepare the key (used both as key A and as key B) using FFFFFFFFFFFF
  // which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
}

short readAsUnsigned(byte b)
{
  short tmp = (short)b;
  if (tmp < 0)
  {
    tmp += 256;
  }
  return tmp;
}

void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

short dump_byte_array_decimal(byte *buffer, byte bufferSize)
{
  // String temp = "";
  short temp = 0;
  Serial.println("Balance: ");
  for (byte i = 0; i < bufferSize; i++)
  {
    temp += (short)readAsUnsigned(buffer[i]);
  }
  Serial.println(temp);
  return temp;
}

void halt()
{
  Serial.println(F("Halting loop"));
  lcd.clear();
  // Halt PICC
  // mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

bool authenticate_card()
{
  // Authenticate using key A
  Serial.println(F("Authenticating using key A"));
  MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    PrintDisplay("Error. Please try again.");
    Serial.println(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    halt();
    return false;
  }
  return true;
}

void write_info(int enterCode, int entryPoint)
{
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Read data from the block
  Serial.print(F("Reading balance from card (block "));
  Serial.print(balanceBlock);
  Serial.println(F(")"));
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(infoBlock, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  dump_byte_array_decimal(buffer, 16);
  dump_byte_array(buffer, 16);

  byte infoData[] = {
      byte(enterCode),
      byte(entryPoint),
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
  };

  status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(infoBlock, infoData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.println(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println(F("Write finished - info"));
  // dump_byte_array(balanceData, 16);

  // status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(balanceBlock, buffer, &size);
  // if (status != MFRC522::STATUS_OK)
  // {
  //   Serial.print(F("MIFARE_Read() failed: "));
  //   Serial.println(mfrc522.GetStatusCodeName(status));
  //   PrintDisplay("Error!");
  // }
  // dump_byte_array_decimal(buffer, 16);
  // dump_byte_array(buffer, 16);
}

bool read_balance()
{
  byte buffer[18];
  byte size = sizeof(buffer);

  // Read data from the block
  Serial.print(F("Reading balance from card (block "));
  Serial.print(balanceBlock);
  Serial.println(F(")"));
  MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  short amount = dump_byte_array_decimal(buffer, 16);

  lcd.setCursor(0, 0);
  lcd.print("Balance: " + String(amount));
}

short get_balance()
{
  byte buffer[18];
  byte size = sizeof(buffer);

  Serial.print(F("Reading balance from card (block "));
  Serial.print(balanceBlock);
  Serial.println(F(")"));
  MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  short amount = dump_byte_array_decimal(buffer, 16);

  return amount;
}

void write_amount_to_card(short amount)
{
  // Serial.println("Reloading up " + String(amount) + ".");
  // PrintDisplay("Reloading up " + String(amount) + ".");

  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Read data from the block
  Serial.print(F("Reading balance from card (block "));
  Serial.print(balanceBlock);
  Serial.println(F(")"));
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(balanceBlock, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  dump_byte_array_decimal(buffer, 16);
  dump_byte_array(buffer, 16);

  byte balanceData[] = {
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
  };

  // Create balance data block
  int ind = amount / 255;
  // Serial.println("ind: " + String(ind));
  if (ind > 16)
    return;
  int leftover = amount % 255;
  for (int i = 0; i < ind; i++)
  {
    balanceData[i] = (byte)0xFF;
  }
  balanceData[ind] = byte(leftover);
  // Serial.println("balanceData: ");
  // for (int i = 0; i < ind; i++)
  // {
  //   Serial.println(balanceData[i]);
  // }

  status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(balanceBlock, balanceData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.println(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println(F("Write finished"));
  lcd.setCursor(0, 3);
  lcd.print("New Bal: " + String(amount));
  delay(2000);
  // dump_byte_array(balanceData, 16);

  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(balanceBlock, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  dump_byte_array_decimal(buffer, 16);
  dump_byte_array(buffer, 16);
}

void ReloadRFID()
{
  PrintDisplay("Reload. Please tap card..");
  while (!mfrc522.PICC_IsNewCardPresent())
  {
  }

  PrintDisplay("Reading...");

  unsigned long currentTime = millis();
  while ((!mfrc522.PICC_ReadCardSerial()) && (millis() - currentTime < 5000))
  {
  }

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  uid.toUpperCase();

  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  }

  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  if (authenticate_card() == true)
  {
    read_balance();
  }
  short amt;
  short currBalance = get_balance();
  while (true)
  {
    amt = getAmountFromKeypad();
    if ((amt + currBalance) < maxReloadAmount)
      break;
    else
    {
      PrintDisplay("Amount must be less than " + String(maxReloadAmount));
      delay(2500);
    }
  }

  Serial.println("Reloading " + (String)amt);
  PrintDisplay("Reloading " + (String)amt);
  write_amount_to_card(amt + currBalance);
  PrintDisplay("Successfully loaded " + (String)amt);
  delay(3000);
  mode = 0;
  halt();
}

void ReadRFID()
{

  PrintDisplay("Please tap card..");

  PrintDisplay("Reading...");

  unsigned long currentTime = millis();
  while ((!mfrc522.PICC_ReadCardSerial()) && (millis() - currentTime < 5000))
  {
  }

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  uid.toUpperCase();

  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
  {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  }

  if (authenticate_card() == true)
  {
    read_balance();
    unsigned long startMillis = millis();
    bool receivedStation = false;
    int destination = 0;
    lcd.setCursor(0, 2);
    lcd.print("Press destination");
    lcd.setCursor(0, 3);
    lcd.print("button.");
    while ((millis() - startMillis <= 10000))
    {
      if (BUTTSerial.available() > 0)
      {
        int val = BUTTSerial.parseInt();
        Serial.println(val);
        if (val != -1 && val < TOTALSTATIONS)
        {
          destination = val;
          receivedStation = true;
          BUTTSerial.print("Ok");
          break;
        }
      }
    }

    if (receivedStation == true)
    {
      if (destination > currentStation)
      {
        PrintDisplay(route[destination].name);
        short fare = (short)FAREBASE + ((short)FAREMULTIPLIER * ((short)destination - (short)currentStation));
        lcd.setCursor(0, 1);
        lcd.print("Fare: " + String(fare));

        short currentBalance = get_balance();
        if (currentBalance > fare)
        {
          write_amount_to_card(get_balance() - fare);
        }
        else
        {
          lcd.setCursor(0, 3);
          lcd.print("INSUFFICIENT BALANCE");
          delay(5000);
          halt();
        }

        Serial.flush();

        // Send data to driver
        StaticJsonDocument<100> doc;
        doc["action"] = "passenger";
        doc["sid"] = destination;
        doc["qty"] = 1;
        String buffer;
        serializeJson(doc, buffer);
        BTSerial.println(buffer);
        BTSerial.flush();
        delay(1500);
        Serial.println(buffer);
        doc.clear();
        // serializeJson(doc, Serial);
        // BTSerial.flush();
        // Serial.flush();

        delay(3000);
      }
      else
      {
        PrintDisplay("Invalid station!");
        delay(2500);
      }
    }
    else
    {
      PrintDisplay("Route button not pressed!");
      delay(2500);
    }
    // if (entered() == false)
    // {
    //   lcd.clear();
    //   lcd.setCursor(0,0);
    //   lcd.print("Entry");
    //   write_info(1, 1);
    //   read_balance();
    // }
    // else if(entered() == true)
    // {
    //   lcd.clear();
    //   lcd.setCursor(0,0);
    //   lcd.print("Exit");
    //   write_info(0,0);
    //   write_amount_to_card(get_balance()-15);
    // }
  }

  // if (authenticate_card() == true)
  // {
  //   if (entered() == false)
  //   {
  //     lcd.clear();
  //     lcd.setCursor(0,0);
  //     lcd.print("Entry");
  //     write_info(1, 1);
  //     read_balance();
  //   }
  //   else if(entered() == true)
  //   {
  //     lcd.clear();
  //     lcd.setCursor(0,0);
  //     lcd.print("Exit");
  //     write_info(0,0);
  //     write_amount_to_card(get_balance()-15);
  //   }
  // }
  halt();
}

void reload_ISR()
{
  // Debounce
  if (millis() - lastDetectedReloadButton > 750)
  {
    // Serial.println("Mode = 1");
    // PrintDisplay("===Reload===");
    mode = 1;
    lastDetectedReloadButton = millis();
  }
}

void reload_init()
{
  pinMode(reloadButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(reloadButton), reload_ISR, FALLING);
}

// Main Loops
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Initialize Modules
  InitDisplay();
  InitBluetooth();
  InitRFID();
  reload_init();
  InitGPS();
  InitButtonArray();
  PrintDisplay("Done!");
  lcd.clear();
}

int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void loop()
{
  if (mode == 0)
  {
    // lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("Jeepaze");
    lcd.setCursor(0, 2);
    lcd.print("Please tap card..");
    if (mfrc522.PICC_IsNewCardPresent())
    {
      ReadRFID();
    }
  }
  else if (mode == 1)
  {
    ReloadRFID();
  }

  // UpdateGPS();  // Uncomment if actual GPS unit.
  SetCurrentStationTest();
  // delay(1000);
}