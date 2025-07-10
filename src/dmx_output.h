#pragma once
#include <Arduino.h>

class DmxOutput {
public:
  explicit DmxOutput(int txPin);
  void begin();
  void setChannel(uint16_t ch, uint8_t value);
  void sendFrame(uint16_t channels = 512);
private:
  int txPin;
  uint8_t buffer[512];
};
