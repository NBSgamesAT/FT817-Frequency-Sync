#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial ft817 = SoftwareSerial(2, 3);

String getMode(uint8_t byte);
unsigned long bcdtoi(uint8_t bcd);

void setup() {
  // put your setup code here, to run once:
  ft817.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t bytes[] = {0x00,0x00,0x00,0x00,0x03};

  for (unsigned int i = 0; i < sizeof(bytes); i++){
    ft817.write(bytes[i]);
    Serial.print(String(bytes[i]));
    delay(1);
  }
  Serial.println();
  uint8_t buffer[5];
  ft817.readBytes(&buffer[0], sizeof(buffer));

  String mode = getMode(buffer[4]);

  //int freq = getFreq(buffer[0], buffer[1], buffer[2], buffer[3]);
  /*char buf[10];
  for(int i = 0; i<4; i++) {
    snprintf(buf, sizeof(buf), "%x ", buffer[i]);
    Serial.print(buf);
  }*/

  //Serial.println();
  unsigned long actualFrequency = 0;
  actualFrequency = bcdtoi(buffer[0]) * 1000000;
  actualFrequency += (bcdtoi(buffer[1]) * 10000);
  actualFrequency += (bcdtoi(buffer[2]) * 100);
  actualFrequency += bcdtoi(buffer[3]);

  
  Serial.print(actualFrequency);

  Serial.write(0x20);
  Serial.println(mode);

  delay(2000);
  //String freq = Serial.readString();
  

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

unsigned long bcdtoi(uint8_t bcd){
  int byte = (bcd & 0xF0) >> 4;
  byte *= 10;
  byte += bcd & 0x0F;
  return byte;
}