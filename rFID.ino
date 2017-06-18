#include <SPI.h>
#include <MFRC522.h> // thu vien "RFID".
const int LED1 = 5;
const int LED2 = 6;

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);       
unsigned long uidDec, uidDecTemp; // hien thi so UID dang thap phan
byte bCounter, readBit;
unsigned long ticketNumber;


void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);     
  SPI.begin();            
  mfrc522.PCD_Init();     
}

void loop() {
  // Tim the moi
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Doc the
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  uidDec = 0;
  // Hien thi so UID cua the
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec*256+uidDecTemp;
  } 
  Serial.println(uidDec);
//  if(uidDec == 548294201){
//    digitalWrite(LED1,!digitalRead(LED1));
//    delay(2000);
//  }else{
//  }
//  //--------------------------------
//  if(uidDec == 2158142588){
//    digitalWrite(LED2,!digitalRead(LED2));
//    delay(2000);
//  }else{
//    // Halt PICC
//    mfrc522.PICC_HaltA();  // dung lai
//    return;
//    delay(1);
//  }
  // Halt PICC
  mfrc522.PICC_HaltA();                      
}


