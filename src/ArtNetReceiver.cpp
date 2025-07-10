// ArtNetReceiver.cpp
#include "ArtNetReceiver.h"

#define ART_NET_PORT 6454
#define ART_DMX_START 18
#define ART_UNIVERSE_LOW 14
#define ART_UNIVERSE_HIGH 15

ArtNetReceiver artnet;

void ArtNetReceiver::begin() {
    udp.begin(ART_NET_PORT);
}

void ArtNetReceiver::update() {
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
        udp.read(packetBuffer, sizeof(packetBuffer));
        handlePacket(packetSize);
    }
}

void ArtNetReceiver::handlePacket(int packetSize) {
    if (memcmp(packetBuffer, "Art-Net", 7) != 0) return;

    uint16_t universe = packetBuffer[ART_UNIVERSE_LOW] | (packetBuffer[ART_UNIVERSE_HIGH] << 8);
    uint8_t* dmxData = &packetBuffer[ART_DMX_START];
    uint16_t length = packetSize - ART_DMX_START;

    if (universe == settings.universe) {
        updateLEDs(dmxData, length);
    }
    if (universe == settings.dmxUniverse) {
        updateDMX(dmxData, length);
    }
}
