#include "settings_manager.h"

SettingsManager::SettingsManager() {}

void SettingsManager::begin() {
  prefs.begin("node", false);
  loadSettings();
}

void SettingsManager::loadSettings() {
  settings.universe = prefs.getUShort("universe", 1);
  settings.startChannel = prefs.getUShort("startCh", 1);
  settings.ledCount = prefs.getUShort("ledCount", DEFAULT_LED_COUNT);
  settings.dmxUniverse = prefs.getUShort("dmxUniv", 1);
  settings.mode = prefs.getUChar("mode", 0);
  settings.overrideR = prefs.getUChar("ovrR", 255);
  settings.overrideG = prefs.getUChar("ovrG", 0);
  settings.overrideB = prefs.getUChar("ovrB", 0);
  settings.isRoot = prefs.getBool("isRoot", false);
  prefs.getString("wifiSSID", settings.wifiSSID, sizeof(settings.wifiSSID));
  prefs.getString("wifiPass", settings.wifiPassword, sizeof(settings.wifiPassword));
}

void SettingsManager::saveSettings() {
  prefs.putUShort("universe", settings.universe);
  prefs.putUShort("startCh", settings.startChannel);
  prefs.putUShort("ledCount", settings.ledCount);
  prefs.putUShort("dmxUniv", settings.dmxUniverse);
  prefs.putUChar("mode", settings.mode);
  prefs.putUChar("ovrR", settings.overrideR);
  prefs.putUChar("ovrG", settings.overrideG);
  prefs.putUChar("ovrB", settings.overrideB);
  prefs.putBool("isRoot", settings.isRoot);
  prefs.putString("wifiSSID", settings.wifiSSID);
  prefs.putString("wifiPass", settings.wifiPassword);
}

NodeSettings& SettingsManager::getSettings() {
  return settings;
}
