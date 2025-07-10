#include "effect_wipe.h"

EffectWipe::EffectWipe() : currentLed(0), color(0) {}

void EffectWipe::begin(Adafruit_NeoPixel& strip) {
  currentLed = 0;
  color = strip.Color(255, 0, 0);
}

void EffectWipe::update(Adafruit_NeoPixel& strip) {
  if (currentLed >= strip.numPixels()) {
    currentLed = 0;
    strip.clear();
  }
  strip.setPixelColor(currentLed, color);
  strip.show();
  currentLed++;
  delay(50);
}
