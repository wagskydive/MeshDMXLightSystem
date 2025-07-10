#include "wifi_manager.h"
#include <WiFi.h>

WiFiManager::WiFiManager() : currentRSSI(-127), lastCheck(0), apMode(false) {
  memset(ssid, 0, sizeof(ssid));
  memset(password, 0, sizeof(password));
}

void WiFiManager::begin(const char* _ssid, const char* _password) {
  strncpy(ssid, _ssid, sizeof(ssid) - 1);
  strncpy(password, _password, sizeof(password) - 1);
  WiFi.mode(WIFI_STA);
  connect();
}

void WiFiManager::connect() {
  if (strlen(ssid) == 0) {
    startAP();
    return;
  }
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect. Starting AP mode.");
    startAP();
  } else {
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
    currentRSSI = WiFi.RSSI();
    apMode = false;
  }
}

void WiFiManager::startAP() {
  Serial.println("Starting Access Point mode...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("LED_Mesh_Setup");
  apMode = true;
  currentRSSI = -127;
  Serial.println("AP IP: " + WiFi.softAPIP().toString());
}

bool WiFiManager::isConnected() const {
  return WiFi.status() == WL_CONNECTED && !apMode;
}

int8_t WiFiManager::getRSSI() const {
  if (isConnected()) return WiFi.RSSI();
  return -127;
}

void WiFiManager::loop() {
  if (isConnected() && millis() - lastCheck > 10000) {
    currentRSSI = WiFi.RSSI();
    lastCheck = millis();
  }
}
