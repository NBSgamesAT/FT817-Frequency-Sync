#include <FT817_NBS.h>

FT817_NBS::FT817_NBS(SoftwareSerial *connection){
  serial = connection;
}

unsigned long FT817_NBS::bcdToInt(uint8_t bcd){
  int byte = (bcd & 0xF0) >> 4;
  byte *= 10;
  byte += bcd & 0x0F;
  return byte;
}

uint8_t FT817_NBS::intToBcd(unsigned long value){
  unsigned long tens = value / 10;
  value -= tens * 10;
  unsigned long singles = value;
  uint8_t returnValue = (tens & 0xF) << 4;
  returnValue = returnValue | (singles & 0xF);
  return returnValue;
}

unsigned long FT817_NBS::getDevicer(int step){
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
    //Serial.println("SHIT"); //CAN'T HAPPEN
    return 1;
    break;
  }
}

FT817_NBS::SignalMode FT817_NBS::getSignalMode(uint8_t numericValue){
  switch (numericValue){
    case 0x0:
      return FT817_NBS::SignalMode::LSB;
      break;
    case 0x01:
      return FT817_NBS::SignalMode::USB;
      break;
    case 0x02:
      return FT817_NBS::SignalMode::CW;
      break;
    case 0x03:
      return FT817_NBS::SignalMode::CWR;
      break;
    case 0x4:
      return FT817_NBS::SignalMode::AM;
      break;
    case 0x6:
      return FT817_NBS::SignalMode::WFM;
      break;
    case 0x8:
      return FT817_NBS::SignalMode::FM;
      break;
    case 0xA:
      return FT817_NBS::SignalMode::DIG;
      break;
    case 0xC:
      return FT817_NBS::SignalMode::PKT;
      break;
    default:
      return FT817_NBS::SignalMode::UNKNOWN;
      break;
  }
}

void FT817_NBS::convertFromValueToBcd(uint8_t *buffer, unsigned long actualValue){
  for(int partsLeft = 0;partsLeft < 4; partsLeft++){
    unsigned long part = actualValue / getDevicer(partsLeft);
    actualValue -= part * getDevicer(partsLeft);
    uint8_t bcd = intToBcd(part);
    buffer[partsLeft] = bcd;
  }
}

unsigned long FT817_NBS::convertFromBcdToValue(uint8_t *buffer){
  unsigned long actualValue = 0;
  for(int partsleft = 0; partsleft < 4; partsleft++){
    unsigned long multiplier = getDevicer(partsleft);
    unsigned long partValue = bcdToInt(buffer[partsleft]);
    actualValue += (partValue * multiplier);
  }
  return actualValue;
}

void FT817_NBS::clearAvailableBytes(){
  if(serial->available() != 0){ //Emptying ft817.available()
    uint8_t throwAwayBuffer[serial->available()];
    serial->readBytes(&throwAwayBuffer[0], serial->available());
  }
}

void FT817_NBS::sendCommand(uint8_t *bytes, size_t len){
  for (unsigned int i = 0; i < len; i++){
    serial->write(bytes[i]);
    //Serial.print(String(bytes[i], HEX));
    delay(1);
  }
}

void FT817_NBS::toggleAB(){
  clearAvailableBytes();
  uint8_t data[] = {0x0,0x0,0x0,0x0,0x81};
  sendCommand(&data[0], sizeof(data));
}

FT817_NBS::Frequency FT817_NBS::getFrequency(){
  clearAvailableBytes();
  uint8_t data[] = {0x0,0x0,0x0,0x0,0x3};
  sendCommand(&data[0], sizeof(data));

  delay(10);
  
  uint8_t buffer[5];
  serial->readBytes(&buffer[0], sizeof(buffer));
  FT817_NBS::Frequency frequency;
  frequency.frequency = convertFromBcdToValue(&buffer[0]);
  frequency.mode = getSignalMode(buffer[4]);
  return frequency;
}

void FT817_NBS::setFrequency(unsigned long frequency){
  clearAvailableBytes();
  uint8_t data[5];
  convertFromValueToBcd(&data[0], frequency);
  data[4] = 0x1;
  sendCommand(&data[0], sizeof(data));
}

void FT817_NBS::setMode(FT817_NBS::SignalMode mode){
  if(mode == SignalMode::UNKNOWN || mode == SignalMode::WFM){
    return;
  }
  uint8_t value = (uint8_t) mode;
  uint8_t data[5] = {value, 0x0, 0x0, 0x0, 0x7};
  sendCommand(&data[0], sizeof(data));
}

void FT817_NBS::setSplit(bool on){
  uint8_t data[5] = {0x0, 0x0, 0x0, 0x0};
  if(on){
    data[4] = 0x2;
  }
  else{
    data[4] = 0x82;
  }
  sendCommand(&data[0], sizeof(data));
}