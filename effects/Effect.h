#ifndef EFFECT_H
#define EFFECT_H

class Effect {
public:
  virtual void begin() = 0;
  virtual void update() = 0;
};

#endif
