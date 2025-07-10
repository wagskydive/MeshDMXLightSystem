// ArtNetReceiver.h
#ifndef ARTNET_RECEIVER_H
#define ARTNET_RECEIVER_H

#include <WiFiUdp.h>
#include "NodeSettings.h"
#include "DMXOutput.h"
#include "LEDOutput.h"

class ArtNetReceiver {
public:
    void begin();
    void update();
private:
    WiFiUDP udp;
    uint8_t packetBuffer[530];
    void handlePacket(int packetSize);
};

extern ArtNetReceiver artnet;

#endif
