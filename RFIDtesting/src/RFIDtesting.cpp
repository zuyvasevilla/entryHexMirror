/*

 */

#include "Particle.h"
#include <SPI.h>
#include <MFRC522.h>


//RFID
int RST_PIN = 12;         
int SS_PIN = 18;
byte readCard[4];
String keytagUID = "5E0453C3";
String cardUID = "29E2F65E";
String scannedID = "";        
MFRC522 READER(SS_PIN, RST_PIN);  // Create MFRC522 instance
bool checkID();




SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	waitFor(Serial.isConnected, 10000);
  	Serial.println("SERIAL WORKS");
	SPI.begin();			// Init SPI bus
	READER.PCD_Init();		// Init MFRC522
	READER.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details

	Serial.println("Scan PICC to see UID, SAK, type, and data blocks...");

}

void loop() {

	if ( ! READER.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! READER.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	READER.PICC_DumpToSerial(&(READER.uid));
}

bool checkID(){
  int i;
	if( ! READER.PICC_IsNewCardPresent()){
		return FALSE;
	}
	if( ! READER.PICC_ReadCardSerial()){
		return FALSE;
	}

	scannedID = "";

	for(i=0; i<4; i++){
		readCard[i] = READER.uid.uidByte[i];
    	scannedID.concat(String(READER.uid.uidByte[i], HEX)); // Convert the UID to a single String
    }
    scannedID.toUpperCase();
    READER.PICC_HaltA(); // Stop reading
    return true;
}
