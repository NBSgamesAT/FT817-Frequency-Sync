#ifndef FT817_NBS_H
#define FT817_NBS_H
#include <Arduino.h>
#include <SoftwareSerial.h>
//#include <Arduino.h>

class FT817_NBS {
public:
  enum SignalMode {
    LSB = 0x0,
    USB = 0x1,
    CW = 0x2,
    CWR = 0x3,
    AM = 0x4,
    WFM = 0x6,
    FM = 0x8,
    DIG = 0xA,
    PKT = 0xC,
    UNKNOWN
  };

private:
  SoftwareSerial *serial;
  unsigned long bcdToInt(uint8_t bcd);
  unsigned long getDevicer(int step);
  uint8_t intToBcd(unsigned long value);
  void convertFromValueToBcd(uint8_t *buffer, unsigned long actualValue);
  unsigned long convertFromBcdToValue(uint8_t *buffer);
  void clearAvailableBytes();
  void sendCommand(uint8_t *data, size_t len);
  FT817_NBS::SignalMode getSignalMode(uint8_t numericValue);

public:
  FT817_NBS(SoftwareSerial *connection);

  struct Frequency {
    unsigned long frequency;
    FT817_NBS::SignalMode mode;
  };

  FT817_NBS::Frequency getFrequency();
  void setFrequency(unsigned long frequency);
  void toggleAB();
  void setMode(FT817_NBS::SignalMode);
  void setSplit(bool on);
};
#endif