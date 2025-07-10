#pragma once
#include <Arduino.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>  // You'll need the ArtnetWifi library

#include "settings_manager.h"
#include <Adafruit_NeoPixel.h>

class DmxOutput;

class ArtNetReceiver {
public:
  ArtNetReceiver(SettingsManager& settings, Adafruit_NeoPixel& strip, DmxOutput* dmxOut = nullptr);
  void begin();
  void handle();

private:
  SettingsManager& settingsManager;
  Adafruit_NeoPixel& ledStrip;
  ArtnetWifi artnet;
  DmxOutput* dmx;
};
