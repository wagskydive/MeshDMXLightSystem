#ifndef TOPOLOGY_ALTERNATING_EFFECT_H
#define TOPOLOGY_ALTERNATING_EFFECT_H

#include <Adafruit_NeoPixel.h>
#include "Effect.h"

class TopologyAlternatingEffect : public Effect {
  Adafruit_NeoPixel* strip;
  uint32_t primaryColor;
  uint32_t secondaryColor;
  uint8_t nodeIndex;
  uint8_t totalNodes;
  bool onState;
  unsigned long lastToggle;
  unsigned long interval;

public:
  TopologyAlternatingEffect(Adafruit_NeoPixel* s)
    : strip(s),
      primaryColor(strip->Color(255, 0, 0)),
      secondaryColor(strip->Color(0, 0, 255)),
      nodeIndex(0),
      totalNodes(1),
      onState(true),
      lastToggle(0),
      interval(500) {}

  void begin() override {
    lastToggle = millis();
  }

  void update() override {
    unsigned long now = millis();
    if (now - lastToggle >= interval) {
      lastToggle = now;
      onState = !onState;
      updateStrip();
    }
  }

  void updateStrip() {
    for (uint16_t i = 0; i < strip->numPixels(); i++) {
      uint32_t color = 0; // off
      if (onState) {
        // Simple pattern: even nodeIndex uses primary, odd uses secondary
        if ((nodeIndex % 2) == 0) {
          color = primaryColor;
        } else {
          color = secondaryColor;
        }
      }
      strip->setPixelColor(i, color);
    }
    strip->show();
  }

  // Setters for mesh parameters and colors
  void setNodeIndex(uint8_t idx) { nodeIndex = idx; }
  void setTotalNodes(uint8_t total) { totalNodes = total; }
  void setPrimaryColor(uint32_t c) { primaryColor = c; }
  void setSecondaryColor(uint32_t c) { secondaryColor = c; }
  void setInterval(unsigned long ms) { interval = ms; }
};

#endif
