#ifndef RAINBOW_EFFECT_H
#define RAINBOW_EFFECT_H

#include "Effect.h"

class RainbowEffect : public Effect {
  private:
    Adafruit_NeoPixel* strip;
    uint16_t currentHue;

  public:
    RainbowEffect(Adafruit_NeoPixel* ledStrip) : strip(ledStrip), currentHue(0) {}

    void begin() override {
      currentHue = 0;
    }

    void update() override {
      for (uint16_t i = 0; i < strip->numPixels(); i++) {
        uint16_t hue = (currentHue + (i * 65536L / strip->numPixels())) % 65536;
        strip->setPixelColor(i, strip->gamma32(strip->ColorHSV(hue)));
      }
      strip->show();
      currentHue += 256; // increment hue (adjust speed here)
    }
};

#endif
