#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "config.h"
#include "settings_manager.h"
#include "wifi_manager.h"
#include "mesh_network.h"
#include "webui_server.h"
#include "effects/effect_wipe.h"
#include "artnet_receiver.h"
#include "dmx_output.h"

SettingsManager settingsManager;
WiFiManager wifiManager;
MeshNetwork* meshNetwork;
WebUIServer* webServer;
ArtNetReceiver* artnetReceiver;
DmxOutput* dmxOutput;

Adafruit_NeoPixel strip(LED_PIN, DEFAULT_LED_COUNT, NEO_GRB + NEO_KHZ800);
EffectWipe effectWipe;

unsigned long lastEffectUpdate = 0;
const unsigned long effectInterval = 50;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting ESP32 Mesh LED Node");

  settingsManager.begin();
  NodeSettings& s = settingsManager.getSettings();

  strip.updateLength(s.ledCount);
  strip.begin();
  strip.show();

  wifiManager.begin(s.wifiSSID, s.wifiPassword);

  meshNetwork = new MeshNetwork(wifiManager, settingsManager);
  meshNetwork->begin();

  dmxOutput = new DmxOutput(DMX_TX_PIN);
  dmxOutput->begin();

  webServer = new WebUIServer(settingsManager, *meshNetwork, wifiManager);
  webServer->begin();

  artnetReceiver = new ArtNetReceiver(settingsManager, strip, dmxOutput);
  artnetReceiver->begin();

  effectWipe.begin(strip);

  Serial.println("Setup complete");
}

void loop() {
  wifiManager.loop();
  meshNetwork->loop();
  webServer->loop();

  NodeSettings& s = settingsManager.getSettings();
  switch (s.mode) {
    case 1: {
      uint32_t color = strip.Color(s.overrideR, s.overrideG, s.overrideB);
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
      }
      strip.show();
      break;
    }
    case 2:
      if (millis() - lastEffectUpdate > effectInterval) {
        effectWipe.update(strip);
        lastEffectUpdate = millis();
      }
      break;
    default:
      artnetReceiver->handle();
      break;
  }
}

