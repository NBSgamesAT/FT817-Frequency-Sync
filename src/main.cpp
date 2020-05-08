#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FT817_NBS.h>

#define SET_FREQ 4
#define SWITCH_AB 5

SoftwareSerial radio = SoftwareSerial(2, 3);
FT817_NBS ft817(&radio);

void setFreq();

void setup() {
  // put your setup code here, to run once:
  radio.begin(9600);
  Serial.begin(9600);
  pinMode(SET_FREQ, INPUT_PULLUP);
  pinMode(SWITCH_AB, INPUT_PULLUP);
}

boolean pressBlockSetFreq = false;
boolean pressBlockSwitchAB = false;

void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(SET_FREQ) == LOW && !pressBlockSetFreq){
    pressBlockSetFreq = true;
    setFreq();
  }
  else if(digitalRead(SET_FREQ) != LOW){
    pressBlockSetFreq = false;
    delay(20);
  }

  if (digitalRead(SWITCH_AB) == LOW && !pressBlockSwitchAB){
    ft817.toggleAB();
    pressBlockSwitchAB = true;
    
  }
  else if(digitalRead(SWITCH_AB) != LOW){
    pressBlockSwitchAB = false;
    delay(20);
  }
}

void setFreq(){
  unsigned long frequency = ft817.getFrequency().frequency;

  if(frequency < 28850000){
    Serial.println("Please go into the 70 cm Band!");
    return;
  }
  delay(300);
  unsigned long newFrequency = frequency - 28850000;

  ft817.toggleAB();
  delay(300);

  ft817.setFrequency(newFrequency);

  delay(300);

  ft817.toggleAB();
}