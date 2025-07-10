#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "settings_manager.h"
#include "wifi_manager.h"
#include "mesh_network.h"
#include "webui_server.h"
#include "effects/effect_wipe.h"
#include "artnet_receiver.h"

#define LED_PIN 13

SettingsManager settingsManager;
WiFiManager wifiManager;
MeshNetwork* meshNetwork;
WebUIServer* webServer;
ArtNetReceiver* artnetReceiver;

Adafruit_NeoPixel strip(DEFAULT_LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

EffectWipe effectWipe;

unsigned long lastEffectUpdate = 0;
const unsigned long effectInterval = 50;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting ESP32 Mesh LED Node with Art-Net");

  settingsManager.begin();
  NodeSettings& s = settingsManager.getSettings();

  strip.updateLength(s.ledCount);
  strip.begin();
  strip.show();

  wifiManager.begin(s.wifiSSID, s.wifiPassword);

  meshNetwork = new MeshNetwork(wifiManager, settingsManager);
  meshNetwork->begin();

  webServer = new WebUIServer(settingsManager, *meshNetwork);
  webServer->begin();

  artnetReceiver = new ArtNetReceiver(settingsManager, strip);
  artnetReceiver->begin();

  effectWipe.begin(strip);

  Serial.println("Setup complete");
}

void loop() {
  wifiManager.loop();
  meshNetwork->loop();
  webServer->loop();
  artnetReceiver->handle();

  if (millis() - lastEffectUpdate > effectInterval) {
    effectWipe.update(strip);
    lastEffectUpdate = millis();
  }
}
