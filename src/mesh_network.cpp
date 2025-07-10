#include "mesh_network.h"
#include <esp_now.h>
#include <WiFi.h>
#include "utils.h"

MeshNetwork* globalMeshPtr = nullptr;

MeshNetwork::MeshNetwork(WiFiManager& wifiMgr, SettingsManager& settingsMgr)
  : wifiManager(wifiMgr), settingsManager(settingsMgr), neighborCount(0), rootLastSeen(0), rootTimeout(8000) {
  memset(selfMac, 0, sizeof(selfMac));
  memset(rootMac, 0, sizeof(rootMac));
}

void onDataRecvCallback(const uint8_t* mac, const uint8_t* data, int len) {
  if (globalMeshPtr) {
    globalMeshPtr->onDataRecv(mac, data, len);
  }
}

void MeshNetwork::begin() {
  WiFi.macAddress(selfMac);
  memcpy(rootMac, selfMac, 6);
  rootLastSeen = millis();
  rootTimeout = 8000;

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataRecvCallback);
  globalMeshPtr = this;

  esp_now_peer_info_t peerInfo = {};
  memset(&peerInfo, 0, sizeof(peerInfo));
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Mesh network started");
}

void MeshNetwork::sendHeartbeat() {
  MeshMessage msg = {};
  msg.type = MSG_HEARTBEAT;
  memcpy(msg.senderMac, selfMac, 6);
  msg.isRoot = settingsManager.getSettings().isRoot;
  NodeSettings& s = settingsManager.getSettings();
  msg.universe = s.universe;
  msg.startChannel = s.startChannel;
  msg.ledCount = s.ledCount;
  msg.wifiRSSI = wifiManager.getRSSI();

  esp_err_t res = esp_now_send(nullptr, (uint8_t*)&msg, sizeof(msg));
  if (res != ESP_OK) {
    Serial.println("Failed to send heartbeat");
  }
}

void MeshNetwork::onDataRecv(const uint8_t* mac, const uint8_t* data, int len) {
  if (len != sizeof(MeshMessage)) return;

  MeshMessage* msg = (MeshMessage*)data;

  if (msg->type != MSG_HEARTBEAT) return;

  // Update or add neighbor
  for (size_t i = 0; i < neighborCount; i++) {
    if (memcmp(neighbors[i].mac, mac, 6) == 0) {
      neighbors[i].lastSeen = millis();
      neighbors[i].isRoot = msg->isRoot;
      neighbors[i].universe = msg->universe;
      neighbors[i].startChannel = msg->startChannel;
      neighbors[i].ledCount = msg->ledCount;
      neighbors[i].wifiRSSI = msg->wifiRSSI;
      electRootNode();
      return;
    }
  }

  // New neighbor
  if (neighborCount < MAX_NEIGHBORS) {
    memcpy(neighbors[neighborCount].mac, mac, 6);
    neighbors[neighborCount].lastSeen = millis();
    neighbors[neighborCount].isRoot = msg->isRoot;
    neighbors[neighborCount].universe = msg->universe;
    neighbors[neighborCount].startChannel = msg->startChannel;
    neighbors[neighborCount].ledCount = msg->ledCount;
    neighbors[neighborCount].wifiRSSI = msg->wifiRSSI;
    neighborCount++;
    electRootNode();
  }
}

void MeshNetwork::electRootNode() {
  // Root election: node with strongest WiFi RSSI and lowest MAC if tie
  uint8_t candidateMac[6];
  int8_t bestRSSI = wifiManager.getRSSI();
  memcpy(candidateMac, selfMac, 6);

  for (size_t i = 0; i < neighborCount; i++) {
    if (neighbors[i].wifiRSSI > bestRSSI ||
       (neighbors[i].wifiRSSI == bestRSSI && macLessThan(neighbors[i].mac, candidateMac))) {
      bestRSSI = neighbors[i].wifiRSSI;
      memcpy(candidateMac, neighbors[i].mac, 6);
    }
  }

  if (memcmp(rootMac, candidateMac, 6) != 0) {
    memcpy(rootMac, candidateMac, 6);
    bool newRoot = memcmp(rootMac, selfMac, 6) == 0;
    settingsManager.getSettings().isRoot = newRoot;
    settingsManager.saveSettings();
    Serial.printf("New root elected: %02X:%02X:%02X:%02X:%02X:%02X %s\n",
      rootMac[0], rootMac[1], rootMac[2], rootMac[3], rootMac[4], rootMac[5],
      newRoot ? "(self)" : "");
  }
  rootLastSeen = millis();
}

bool MeshNetwork::isRoot() const {
  return settingsManager.getSettings().isRoot;
}

const MeshNodeInfo* MeshNetwork::getNeighbors() const {
  return neighbors;
}

size_t MeshNetwork::getNeighborCount() const {
  return neighborCount;
}

const uint8_t* MeshNetwork::getRootMac() const {
  return rootMac;
}

void MeshNetwork::loop() {
  sendHeartbeat();

  // Remove stale neighbors
  uint32_t now = millis();
  for (size_t i = 0; i < neighborCount; ) {
    if (now - neighbors[i].lastSeen > 10000) {
      // Remove neighbor by shifting
      for (size_t j = i; j < neighborCount -1; j++) {
        neighbors[j] = neighbors[j+1];
      }
      neighborCount--;
    } else {
      i++;
    }
  }

  // If root timeout -> force re-election
  if (now - rootLastSeen > rootTimeout) {
    electRootNode();
  }
}
