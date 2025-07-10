#pragma once
#include "effect_base.h"

class EffectWipe : public EffectBase {
public:
  EffectWipe();
  void begin(Adafruit_NeoPixel& strip) override;
  void update(Adafruit_NeoPixel& strip) override;

private:
  uint16_t currentLed;
  uint32_t color;
};

