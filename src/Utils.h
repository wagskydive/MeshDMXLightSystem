#pragma once
#include <Arduino.h>

inline bool macLessThan(const uint8_t* mac1, const uint8_t* mac2) {
  for (int i = 0; i < 6; i++) {
    if (mac1[i] < mac2[i]) return true;
    if (mac1[i] > mac2[i]) return false;
  }
  return false;
}

inline void macToString(const uint8_t* mac, char* buffer) {
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
