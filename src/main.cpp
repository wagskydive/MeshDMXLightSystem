#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define LED_PIN    13
#define DEFAULT_LED_COUNT  150

Preferences prefs;
AsyncWebServer server(80);
Adafruit_NeoPixel strip(DEFAULT_LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

struct NodeSettings {
  uint16_t universe = 1;
  uint16_t startChannel = 1;
  uint16_t ledCount = DEFAULT_LED_COUNT;
  uint16_t dmxUniverse = 1;
  uint8_t mode = 0; // 0 = mesh, 1 = color override, 2 = effect override, 3 = manual
  bool isRoot = false;
} settings;

String wifiSSID = "";
String wifiPassword = "";
bool inAPMode = false;

void loadWiFiSettings() {
  prefs.begin("wifi", true);
  wifiSSID = prefs.getString("ssid", "");
  wifiPassword = prefs.getString("pass", "");
  prefs.end();
}

void saveWiFiSettings(String ssid, String pass) {
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
}

void startAPMode() {
  String apName = "LEDNode_" + String((uint32_t)ESP.getEfuseMac(), HEX);
  WiFi.softAP(apName.c_str(), nullptr); // Open AP
  IPAddress IP = WiFi.softAPIP();
  Serial.println("Started AP mode");
  Serial.print("AP SSID: "); Serial.println(apName);
  Serial.print("AP IP address: "); Serial.println(IP);
  inAPMode = true;
}

void connectToWiFi() {
  loadWiFiSettings();
  if (wifiSSID == "") {
    Serial.println("No saved WiFi config. Starting AP mode...");
    startAPMode();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());

  Serial.print("Connecting to WiFi");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection failed. Starting AP mode...");
    startAPMode();
  } else {
    Serial.println("\nConnected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void loadSettings() {
  prefs.begin("node", true);
  settings.universe = prefs.getUShort("universe", 1);
  settings.startChannel = prefs.getUShort("startCh", 1);
  settings.ledCount = prefs.getUShort("ledCount", DEFAULT_LED_COUNT);
  settings.dmxUniverse = prefs.getUShort("dmxUniv", 1);
  settings.mode = prefs.getUChar("mode", 0);
  settings.isRoot = prefs.getBool("isRoot", false);
  prefs.end();
}

void saveSettings() {
  prefs.begin("node", false);
  prefs.putUShort("universe", settings.universe);
  prefs.putUShort("startCh", settings.startChannel);
  prefs.putUShort("ledCount", settings.ledCount);
  prefs.putUShort("dmxUniv", settings.dmxUniverse);
  prefs.putUChar("mode", settings.mode);
  prefs.putBool("isRoot", settings.isRoot);
  prefs.end();
}

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"universe\":" + String(settings.universe) + ",";
    json += "\"startChannel\":" + String(settings.startChannel) + ",";
    json += "\"ledCount\":" + String(settings.ledCount) + ",";
    json += "\"dmxUniverse\":" + String(settings.dmxUniverse) + ",";
    json += "\"mode\":" + String(settings.mode) + ",";
    json += "\"isRoot\":" + String(settings.isRoot ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  });

  server.on("/settings", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("universe", true))
      settings.universe = request->getParam("universe", true)->value().toInt();
    if (request->hasParam("startChannel", true))
      settings.startChannel = request->getParam("startChannel", true)->value().toInt();
    if (request->hasParam("ledCount", true))
      settings.ledCount = request->getParam("ledCount", true)->value().toInt();
    if (request->hasParam("dmxUniverse", true))
      settings.dmxUniverse = request->getParam("dmxUniverse", true)->value().toInt();
    if (request->hasParam("mode", true))
      settings.mode = request->getParam("mode", true)->value().toInt();
    if (request->hasParam("isRoot", true))
      settings.isRoot = request->getParam("isRoot", true)->value() == "true";
    saveSettings();
    request->send(200, "application/json", "{\"status\":\"saved\"}");
  });

  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/wifi.html", "text/html");
  });

  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String newSSID = request->getParam("ssid", true)->value();
      String newPASS = request->getParam("pass", true)->value();
      saveWiFiSettings(newSSID, newPASS);
      request->send(200, "text/plain", "WiFi settings saved. Rebooting...");
      delay(2000);
      ESP.restart();
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  // New: WiFi Scan Endpoint
  server.on("/wifiscan", HTTP_GET, [](AsyncWebServerRequest *request){
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; i++) {
      json += "{";
      json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
      json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
      json += "\"encryption\":" + String(WiFi.encryptionType(i));
      json += "}";
      if (i != n -1) json += ",";
    }
    json += "]";
    WiFi.scanDelete(); // clean up
    request->send(200, "application/json", json);
  });

  server.begin();
}

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  connectToWiFi();

  loadSettings();
  strip.updateLength(settings.ledCount);
  strip.begin();
  strip.show();

  setupWebServer();

  if (inAPMode) {
    Serial.println("WebUI: http://192.168.4.1");
  } else {
    Serial.println("WebUI: http://" + WiFi.localIP().toString());
  }
}

void loop() {
  delay(1000);
}
