#include <Arduino.h>
#include <SoftwareSerial.h>

#define SET_FREQ 4
#define SWITCH_AB 5

SoftwareSerial ft817 = SoftwareSerial(2, 3);

String getMode(uint8_t byte);
unsigned long bcdToInt(uint8_t bcd);
void setFreq();
void switchAB();
void convertFromValueToBCD(uint8_t *buffer, unsigned long actualValue);


void setup() {
  // put your setup code here, to run once:
  ft817.begin(9600);
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
  }

  if (digitalRead(SWITCH_AB) == LOW && !pressBlockSwitchAB){
    switchAB();
    pressBlockSwitchAB = true;
    
  }
  else if(digitalRead(SWITCH_AB) != LOW){
    pressBlockSwitchAB = false;
  }
  //String freq = Serial.readString();
  

}

void sendCommand(uint8_t *bytes, size_t len){
  for (unsigned int i = 0; i < len; i++){
    ft817.write(bytes[i]);
    Serial.print(String(bytes[i], HEX));
    delay(1);
  }
}

void switchAB(){
  Serial.println(ft817.available());
  if(ft817.available() != 0){ //Emptying ft817.available()
    uint8_t throwAwayBuffer[ft817.available()];
    ft817.readBytes(&throwAwayBuffer[0], ft817.available());
  }
  uint8_t bytes[] = {0x00,0x00,0x00,0x00,0x81};
  sendCommand(&bytes[0], sizeof(bytes));
  Serial.println();
}

void setFreq(){
  Serial.println(ft817.available());
  if(ft817.available() != 0){ //Emptying ft817.available()
    uint8_t throwAwayBuffer[ft817.available()];
    ft817.readBytes(&throwAwayBuffer[0], ft817.available());
  }

  uint8_t bytes[] = {0x00,0x00,0x00,0x00,0x03};
  sendCommand(&bytes[0], sizeof(bytes));
  //return;
  
  Serial.println();
  uint8_t buffer[5];
  ft817.readBytes(&buffer[0], sizeof(buffer));

  String mode = getMode(buffer[4]);

  Serial.print(String(buffer[0], HEX) + " ");
  Serial.print(String(buffer[1], HEX) + " ");
  Serial.print(String(buffer[2], HEX) + " ");
  Serial.print(String(buffer[3], HEX) + " ");
  Serial.println(String(buffer[4], HEX));

  unsigned long receiveFrequency = 0;
  receiveFrequency = bcdToInt(buffer[0]) * 1000000;
  receiveFrequency += (bcdToInt(buffer[1]) * 10000);
  receiveFrequency += (bcdToInt(buffer[2]) * 100);
  receiveFrequency += bcdToInt(buffer[3]);

  if (receiveFrequency < 28850000){
    
    Serial.println("Please go into the 70cm Band! Data: " + String(receiveFrequency));
    return;
  }

  unsigned long txFrequency = receiveFrequency - 28850000;

  uint8_t sendBuffer[5];
  convertFromValueToBCD(&sendBuffer[0], txFrequency);

  
  Serial.print(receiveFrequency);
  Serial.write(0x20);
  Serial.println(mode);
  Serial.println(txFrequency);

  Serial.print(String(sendBuffer[0], HEX) + " ");
  Serial.print(String(sendBuffer[1], HEX) + " ");
  Serial.print(String(sendBuffer[2], HEX) + " ");
  Serial.println(String(sendBuffer[3], HEX));

  bytes[4] = 0x81;
  sendCommand(&bytes[0], sizeof(bytes));
  Serial.println();
  delay(100);

  sendBuffer[4] = 0x01;
  sendCommand(&sendBuffer[0], sizeof(sendBuffer));
  Serial.println();
  delay(100);

  sendCommand(&bytes[0], sizeof(bytes));
  Serial.println();
}

String getMode(uint8_t byte){
  switch (byte)
  {
  case 0x00:
    return "LSB";
    break;
  case 0x01:
    return "USB";
    break;
  case 0x02:
    return "CW";
  case 0x03:
    return "CWR";
  case 0x04:
    return "AM";
  case 0x06:
    return "WFM";
  case 0x08:
    return "FM";
  case 0x0A:
    return "DIG";
  case 0x0C:
    return "PKT";
  default:
    return "IDENTIFICATION FAILURE";
    break;
  }
}

unsigned long bcdToInt(uint8_t bcd){
  int byte = (bcd & 0xF0) >> 4;
  byte *= 10;
  byte += bcd & 0x0F;
  return byte;
}

unsigned long getDevicer(int step){
  switch (step) {
  case 0:
    return 1000000;
    break;
  case 1:
    return 10000;
    break;
  case 2:
    return 100;
    break;
  case 3:
    return 1;
    break;
  default:
    Serial.println("SHIT"); //CAN'T HAPPEN
    return 1;
    break;
  }
}

uint8_t intToBcd(unsigned long value){
  unsigned long tens = value / 10;
  value -= tens * 10;
  unsigned long singles = value;
  uint8_t returnValue = (tens & 0xF) << 4;
  returnValue = returnValue | (singles & 0xF);
  return returnValue;
}

void convertFromValueToBCD(uint8_t *buffer, unsigned long actualValue){
  for(int partsLeft = 0;partsLeft < 4; partsLeft++){
    unsigned long part = actualValue / getDevicer(partsLeft);
    actualValue -= part * getDevicer(partsLeft);
    uint8_t bcd = intToBcd(part);
    buffer[partsLeft] = bcd;
  }
}

