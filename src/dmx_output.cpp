#include "dmx_output.h"
#include <HardwareSerial.h>

DmxOutput::DmxOutput(int tx) : txPin(tx) {
  memset(buffer, 0, sizeof(buffer));
}

void DmxOutput::begin() {
  Serial1.begin(250000, SERIAL_8N2, -1, txPin);
}

void DmxOutput::setChannel(uint16_t ch, uint8_t value) {
  if (ch == 0 || ch > 512) return;
  buffer[ch-1] = value;
}

void DmxOutput::sendFrame(uint16_t channels) {
  if (channels > 512) channels = 512;
  Serial1.write(0); // start code
  Serial1.write(buffer, channels);
}

