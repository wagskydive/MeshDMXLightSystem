#pragma once
#include <Preferences.h>
#include <Arduino.h>

struct NodeSettings {
  uint16_t universe = 1;
  uint16_t startChannel = 1;
  uint16_t ledCount = DEFAULT_LED_COUNT;
  uint16_t dmxUniverse = 1;
  uint8_t mode = 0;   // 0=mesh, 1=color override, 2=effect override, 3=manual
  bool isRoot = false;
  char wifiSSID[32] = "";
  char wifiPassword[64] = "";
};

class SettingsManager {
public:
  SettingsManager();
  void begin();
  void loadSettings();
  void saveSettings();
  NodeSettings& getSettings();
private:
  Preferences prefs;
  NodeSettings settings;
};
