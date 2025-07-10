#pragma once
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "settings_manager.h"
#include "mesh_network.h"
#include "wifi_manager.h"

class WebUIServer {
public:
  WebUIServer(SettingsManager& settingsMgr, MeshNetwork& meshNet, WiFiManager& wifiMgr);
  void begin();
  void loop();

private:
  SettingsManager& settingsManager;
  MeshNetwork& meshNetwork;
  WiFiManager& wifiManager;
  AsyncWebServer server;
  void setupRoutes();
};
