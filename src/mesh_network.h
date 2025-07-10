#pragma once
#include <Arduino.h>
#include "wifi_manager.h"
#include "settings_manager.h"

#define MAX_NEIGHBORS 20
#define MSG_HEARTBEAT 1

struct MeshNodeInfo {
  uint8_t mac[6];
  uint32_t lastSeen;
  bool isRoot;
  uint16_t universe;
  uint16_t startChannel;
  uint16_t ledCount;
  int8_t wifiRSSI;
};

struct MeshMessage {
  uint8_t type;
  uint8_t senderMac[6];
  bool isRoot;
  uint16_t universe;
  uint16_t startChannel;
  uint16_t ledCount;
  int8_t wifiRSSI;
};

class MeshNetwork {
public:
  MeshNetwork(WiFiManager& wifiMgr, SettingsManager& settingsMgr);
  void begin();
  void loop();
  bool isRoot() const;
  const MeshNodeInfo* getNeighbors() const;
  size_t getNeighborCount() const;
  const uint8_t* getRootMac() const;

private:
  WiFiManager& wifiManager;
  SettingsManager& settingsManager;
  MeshNodeInfo neighbors[MAX_NEIGHBORS];
  size_t neighborCount;
  uint8_t selfMac[6];
  uint8_t rootMac[6];
  uint32_t rootLastSeen;
  uint32_t rootTimeout;

  void sendHeartbeat();
  void onDataRecv(const uint8_t* mac, const uint8_t* data, int len);
  void electRootNode();
};
