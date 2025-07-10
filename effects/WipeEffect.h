#ifndef WIPE_EFFECT_H
#define WIPE_EFFECT_H

#include "Effect.h"

class WipeEffect : public Effect {
  private:
    Adafruit_NeoPixel* strip;
    uint16_t currentPixel;
    bool forward;
    uint32_t color;

  public:
    WipeEffect(Adafruit_NeoPixel* ledStrip, uint32_t wipeColor = 0x00FF00) : strip(ledStrip), currentPixel(0), forward(true), color(wipeColor) {}

    void begin() override {
      currentPixel = 0;
      forward = true;
      strip->clear();
      strip->show();
    }

    void update() override {
      strip->clear();
      if (forward) {
        strip->setPixelColor(currentPixel, color);
        currentPixel++;
        if (currentPixel >= strip->numPixels()) {
          currentPixel = strip->numPixels() - 1;
          forward = false;
        }
      } else {
        strip->setPixelColor(currentPixel, color);
        if (currentPixel == 0) {
          forward = true;
        } else {
          currentPixel--;
        }
      }
      strip->show();
    }
};

#endif
