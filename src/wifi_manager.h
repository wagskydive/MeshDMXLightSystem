#pragma once
#include <Arduino.h>

class WiFiManager {
public:
  WiFiManager();
  void begin(const char* ssid, const char* password);
  void connect();
  void startAP();
  bool isConnected() const;
  int8_t getRSSI() const;
  void loop();
private:
  char ssid[32];
  char password[64];
  int8_t currentRSSI;
  unsigned long lastCheck;
  bool apMode;
};
