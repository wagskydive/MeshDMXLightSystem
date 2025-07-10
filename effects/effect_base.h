#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class EffectBase {
public:
  virtual ~EffectBase() {}
  virtual void begin(Adafruit_NeoPixel& strip) = 0;
  virtual void update(Adafruit_NeoPixel& strip) = 0;
};
