#pragma once
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "settings_manager.h"
#include "mesh_network.h"

class WebUIServer {
public:
  WebUIServer(SettingsManager& settingsMgr, MeshNetwork& meshNet);
  void begin();
  void loop();

private:
  SettingsManager& settingsManager;
  MeshNetwork& meshNetwork;
  AsyncWebServer server;
  void setupRoutes();
};
