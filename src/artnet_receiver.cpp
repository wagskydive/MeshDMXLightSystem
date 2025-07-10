#include "artnet_receiver.h"

ArtNetReceiver::ArtNetReceiver(SettingsManager& settings, Adafruit_NeoPixel& strip)
  : settingsManager(settings), ledStrip(strip) {}

void ArtNetReceiver::begin() {
  WiFiUDP::stopAll();
  artnet.begin();
  Serial.println("Art-Net receiver started");
}

void ArtNetReceiver::handle() {
  int packetSize = artnet.read();
  if (packetSize > 0) {
    if (artnet.getOpcode() == ART_DMX) {
      uint16_t universe = artnet.getUniverse();
      NodeSettings& s = settingsManager.getSettings();
      if (universe == s.dmxUniverse) {
        const uint8_t* data = artnet.getDMX();
        uint16_t start = s.startChannel - 1;
        uint16_t ledCount = s.ledCount;
        for (uint16_t i = 0; i < ledCount; i++) {
          uint16_t dmxIndex = start + i * 3;
          if (dmxIndex + 2 < artnet.getLength()) {
            uint8_t r = data[dmxIndex];
            uint8_t g = data[dmxIndex + 1];
            uint8_t b = data[dmxIndex + 2];
            ledStrip.setPixelColor(i, ledStrip.Color(r, g, b));
          }
        }
        ledStrip.show();
      }
    }
  }
}
