#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define greenLed 7
#define redLed 6
#define buzzer 5
#define accessPin 4 // Output to STM32 PB11

byte storedUID[4];
bool uidLearned = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(accessPin, OUTPUT);

  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(accessPin, LOW); // LOW = no access

  Serial.println("Place your FIRST RFID card to set it as the correct one...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Learn first card UID
  if (!uidLearned) {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      storedUID[i] = mfrc522.uid.uidByte[i];
    }
    uidLearned = true;
    Serial.println("✅ UID learned! This card will now be accepted.");
    flashLed(greenLed, 3, 150);
  } else {
    bool isSame = true;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (storedUID[i] != mfrc522.uid.uidByte[i]) {
        isSame = false;
        break;
      }
    }

    if (isSame) {
      Serial.println("✅ Correct card detected!");
      successSignal();
      digitalWrite(accessPin, HIGH); // Grant access
    } else {
      Serial.println("❌ Wrong card!");
      errorSignal();
      digitalWrite(accessPin, LOW); // Deny access
    }
  }

  mfrc522.PICC_HaltA();
}

void flashLed(int pin, int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayTime);
    digitalWrite(pin, LOW);
    delay(delayTime);
  }
}

void successSignal() {
  digitalWrite(greenLed, HIGH);
  tone(buzzer, 1000);
  delay(200);
  noTone(buzzer);
  digitalWrite(greenLed, LOW);
}

void errorSignal() {
  digitalWrite(redLed, HIGH);
  tone(buzzer, 400);
  delay(1000);
  noTone(buzzer);
  digitalWrite(redLed, LOW);
}
