#include "webui_server.h"
#include <SPIFFS.h>

WebUIServer::WebUIServer(SettingsManager& settingsMgr, MeshNetwork& meshNet)
  : settingsManager(settingsMgr), meshNetwork(meshNet), server(80) {}

void WebUIServer::begin() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }
  setupRoutes();
  server.begin();
  Serial.println("Web UI started at port 80");
}

void WebUIServer::setupRoutes() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
    NodeSettings& s = settingsManager.getSettings();
    String json = "{";
    json += "\"universe\":" + String(s.universe) + ",";
    json += "\"startChannel\":" + String(s.startChannel) + ",";
    json += "\"ledCount\":" + String(s.ledCount) + ",";
    json += "\"dmxUniverse\":" + String(s.dmxUniverse) + ",";
    json += "\"mode\":" + String(s.mode) + ",";
    json += "\"isRoot\":" + String(s.isRoot ? "true" : "false") + ",";
    json += "\"wifiSSID\":\"" + String(s.wifiSSID) + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  server.on("/settings", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("universe", true))
      settingsManager.getSettings().universe = request->getParam("universe", true)->value().toInt();
    if (request->hasParam("startChannel", true))
      settingsManager.getSettings().startChannel = request->getParam("startChannel", true)->value().toInt();
    if (request->hasParam("ledCount", true))
      settingsManager.getSettings().ledCount = request->getParam("ledCount", true)->value().toInt();
    if (request->hasParam("dmxUniverse", true))
      settingsManager.getSettings().dmxUniverse = request->getParam("dmxUniverse", true)->value().toInt();
    if (request->hasParam("mode", true))
      settingsManager.getSettings().mode = request->getParam("mode", true)->value().toInt();
    if (request->hasParam("wifiSSID", true))
      strncpy(settingsManager.getSettings().wifiSSID, request->getParam("wifiSSID", true)->value().c_str(), sizeof(settingsManager.getSettings().wifiSSID) - 1);
    if (request->hasParam("wifiPassword", true))
      strncpy(settingsManager.getSettings().wifiPassword, request->getParam("wifiPassword", true)->value().c_str(), sizeof(settingsManager.getSettings().wifiPassword) - 1);

    settingsManager.saveSettings();
    request->send(200, "application/json", "{\"status\":\"saved\"}");
  });
}

void WebUIServer::loop() {
  // Nothing needed here for async server
}
