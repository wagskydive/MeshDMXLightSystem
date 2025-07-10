#ifndef STROBE_EFFECT_H
#define STROBE_EFFECT_H

#include "Effect.h"

class StrobeEffect : public Effect {
  private:
    Adafruit_NeoPixel* strip;
    bool on = false;

  public:
    StrobeEffect(Adafruit_NeoPixel* ledStrip) : strip(ledStrip) {}

    void begin() override {
      on = false;
      strip->clear();
      strip->show();
    }

    void update() override {
      on = !on;
      uint32_t color = on ? strip->Color(255, 255, 255) : 0;
      for (uint16_t i = 0; i < strip->numPixels(); i++) {
        strip->setPixelColor(i, color);
      }
      strip->show();
    }
};

#endif
