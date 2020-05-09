#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FT817_NBS.h>

#define SET_FREQ 4
#define SWITCH_AB 5
#define SWITCH_USB_CW 7
#define INIT 9
#define LOWER_FREQ_BEACON 6
#define HIGHER_FREQ_BEACON 8

SoftwareSerial radio = SoftwareSerial(2, 3);
FT817_NBS ft817(&radio);

void setFreq();
void switchUsbCw();
void initRadio();
bool setBeaconHigh();
bool setBeaconLow();

void setup() {
  // put your setup code here, to run once:
  radio.begin(9600);
  Serial.begin(9600);
  pinMode(SET_FREQ, INPUT_PULLUP);
  pinMode(SWITCH_AB, INPUT_PULLUP);
  pinMode(SWITCH_USB_CW, INPUT_PULLUP);
  pinMode(INIT, INPUT_PULLUP);
  pinMode(HIGHER_FREQ_BEACON, INPUT_PULLUP);
  pinMode(LOWER_FREQ_BEACON, INPUT_PULLUP);
}

bool pressBlockSetFreq = false;
bool pressBlockSwitchAB = false;
bool pressBlockSwitchUsbCW = false;
bool pressBlockInit = false;
bool pressBlockLowerFreqBeacon = false;
bool pressBlockHigherFreqBeacon = false;

unsigned long pressLengthInit = 0;

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

  if (digitalRead(SWITCH_USB_CW) == LOW && !pressBlockSwitchUsbCW){
    switchUsbCw();
    pressBlockSwitchUsbCW = true;
    
  }
  else if(digitalRead(SWITCH_USB_CW) != LOW){
    pressBlockSwitchUsbCW = false;
    delay(20);
  }
  
  if(digitalRead(INIT) == LOW && !pressBlockInit){
    pressLengthInit = millis() + 1000;
    pressBlockInit = true;
  }
  else if(digitalRead(INIT) == LOW) {
    if(pressLengthInit != 0 && pressLengthInit <= millis()){
      pressLengthInit = 0;
      initRadio();
    }
  }
  else{
    pressLengthInit = 0;
    pressBlockInit = false;
    delay(20);
  }

  if (digitalRead(HIGHER_FREQ_BEACON) == LOW && !pressBlockHigherFreqBeacon){
    setBeaconHigh();
    pressBlockHigherFreqBeacon = true;
    
  }
  else if(digitalRead(HIGHER_FREQ_BEACON) != LOW){
    pressBlockHigherFreqBeacon = false;
    delay(20);
  }

  if (digitalRead(LOWER_FREQ_BEACON) == LOW && !pressBlockLowerFreqBeacon){
    setBeaconLow();
    pressBlockLowerFreqBeacon = true;
    
  }
  else if(digitalRead(LOWER_FREQ_BEACON) != LOW){
    pressBlockLowerFreqBeacon = false;
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

void switchUsbCw(){
  FT817_NBS::SignalMode mode = ft817.getFrequency().mode;
  FT817_NBS::SignalMode newMode;

  if (mode == FT817_NBS::SignalMode::USB){
    newMode = FT817_NBS::SignalMode::CW;
  }
  else{
    newMode = FT817_NBS::SignalMode::USB;
  }

  delay(300);
  ft817.setMode(newMode);
  
  delay(300);
  ft817.toggleAB();

  delay(300);
  ft817.setMode(newMode);
  
  delay(300);
  ft817.toggleAB();
}

void initRadio(){
  ft817.setMode(FT817_NBS::SignalMode::USB);
  delay(300);
  ft817.setFrequency(14420000);
  delay(300);
  ft817.toggleAB();
  delay(300);
  ft817.setMode(FT817_NBS::SignalMode::USB);
  delay(300);
  ft817.setFrequency(43250000); //43274500, 43250000
  delay(300);
  ft817.setSplit(true);
}

bool setBeacon(unsigned long frequency, FT817_NBS::SignalMode mode, bool firstVFO){
  unsigned long result = ft817.getFrequency().frequency;
  if(result >= 43000000 && result <= 44000000){
    delay(300);
    ft817.setMode(mode);
    delay(300);
    ft817.setFrequency(frequency);
    return true;
  }
  else{
    delay(300);
    ft817.toggleAB();
    if(firstVFO){
      delay(300);
      return setBeacon(frequency, mode, false);
    }
    else{
      Serial.println("Could not set beacon. Please initialise the radio first.");
      return false;
    }
  }
}

bool setBeaconHigh(){
  return setBeacon(43274500, FT817_NBS::SignalMode::USB, true);
}

bool setBeaconLow(){
  return setBeacon(43250000, FT817_NBS::SignalMode::CW, true);
}


