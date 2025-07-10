#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN    13
#define LED_COUNT  150

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Node started.");
}

void loop() {
  // Placeholder for main functionality
  // Node will eventually receive Art-Net or effect data here
  delay(1000);
}
