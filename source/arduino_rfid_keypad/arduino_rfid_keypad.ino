#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

Servo myservo;
int pos = 0;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address

void setup() 
{
  Serial.begin(9600); // Initiate a serial communication
  SPI.begin(); // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  pinMode(8, OUTPUT);
  myservo.attach(6);

  lcd.init(); // Initialize the lcd 
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() 
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  // Show UID on serial monitor and LCD
  Serial.print("UID tag :");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID tag :");
  
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  lcd.setCursor(0, 1);
  lcd.print(content.substring(1));

  // Send UID to ESP32
  Serial.print("UID: ");
  Serial.println(content.substring(1));

  if (content.substring(1) == "A3 DB EB 0D" || content.substring(1) == "7E 32 30 00" || content.substring(1) == "44 7E 37 FE" || content.substring(1) == "95 79 1C 53" || content.substring(1) == "F5 BC 0C 53" || content.substring(1) == "F7 73 A1 D5") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print(content.substring(1));
    
    digitalWrite(8, HIGH);
    for (pos = 0; pos <= 180; pos += 1) {
      myservo.write(pos);
      delay(5);
    }
    delay(1000);
    for (pos = 180; pos >= 0; pos -= 1) {
      myservo.write(pos);
      delay(5);
    }
    digitalWrite(8, LOW);
  } 
  else 
  {
    Serial.println("Access denied");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    delay(1000);
  }
}