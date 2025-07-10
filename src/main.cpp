#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define LED_PIN    13
#define DEFAULT_LED_COUNT  150

Preferences prefs;
AsyncWebServer server(80);
Adafruit_NeoPixel strip(DEFAULT_LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

struct NodeSettings {
  uint16_t universe = 1;
  uint16_t startChannel = 1;
  uint16_t ledCount = DEFAULT_LED_COUNT;
  uint16_t dmxUniverse = 1;
  uint8_t mode = 0; // 0 = mesh, 1 = color override, 2 = effect override, 3 = manual
  bool isRoot = false;
} settings;

struct MeshNodeInfo {
  uint8_t mac[6];
  uint32_t lastSeen; // millis()
  bool isRoot;
  uint16_t universe;
  uint16_t startChannel;
  uint16_t ledCount;
};

#define MAX_NEIGHBORS 10
MeshNodeInfo neighbors[MAX_NEIGHBORS];
uint8_t neighborCount = 0;

uint8_t selfMac[6];
bool inAPMode = false;

enum MeshMsgType : uint8_t {
  MSG_HEARTBEAT = 1,
  MSG_STATE_BROADCAST = 2,
};

struct MeshMessage {
  uint8_t type;
  uint8_t senderMac[6];
  bool isRoot;
  uint16_t universe;
  uint16_t startChannel;
  uint16_t ledCount;
};

String wifiSSID = "";
String wifiPassword = "";

void loadWiFiSettings() {
  prefs.begin("wifi", true);
  wifiSSID = prefs.getString("ssid", "");
  wifiPassword = prefs.getString("pass", "");
  prefs.end();
}

void saveWiFiSettings(String ssid, String pass) {
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
}

void loadSettings() {
  prefs.begin("node", true);
  settings.universe = prefs.getUShort("universe", 1);
  settings.startChannel = prefs.getUShort("startCh", 1);
  settings.ledCount = prefs.getUShort("ledCount", DEFAULT_LED_COUNT);
  settings.dmxUniverse = prefs.getUShort("dmxUniv", 1);
  settings.mode = prefs.getUChar("mode", 0);
  settings.isRoot = prefs.getBool("isRoot", false);
  prefs.end();
}

void saveSettings() {
  prefs.begin("node", false);
  prefs.putUShort("universe", settings.universe);
  prefs.putUShort("startCh", settings.startChannel);
  prefs.putUShort("ledCount", settings.ledCount);
  prefs.putUShort("dmxUniv", settings.dmxUniverse);
  prefs.putUChar("mode", settings.mode);
  prefs.putBool("isRoot", settings.isRoot);
  prefs.end();
}

void startAPMode() {
  String apName = "LEDNode_" + String((uint32_t)ESP.getEfuseMac(), HEX);
  WiFi.softAP(apName.c_str(), nullptr); // Open AP
  IPAddress IP = WiFi.softAPIP();
  Serial.println("Started AP mode");
  Serial.print("AP SSID: "); Serial.println(apName);
  Serial.print("AP IP address: "); Serial.println(IP);
  inAPMode = true;
}

void connectToWiFi() {
  loadWiFiSettings();
  if (wifiSSID == "") {
    Serial.println("No saved WiFi config. Starting AP mode...");
    startAPMode();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());

  Serial.print("Connecting to WiFi");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection failed. Starting AP mode...");
    startAPMode();
  } else {
    Serial.println("\nConnected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

uint32_t rootLastSeen = 0;
uint8_t rootMac[6] = {0,0,0,0,0,0};

bool macLessThan(const uint8_t *a, const uint8_t *b) {
  for (int i = 0; i < 6; i++) {
    if (a[i] < b[i]) return true;
    if (a[i] > b[i]) return false;
  }
  return false; // equal
}

void onDataRecv(const uint8_t * mac_addr, const uint8_t *data, int len) {
  if (len < sizeof(MeshMessage)) return;
  MeshMessage msg;
  memcpy(&msg, data, sizeof(MeshMessage));

  // Update neighbor info
  bool found = false;
  for (int i = 0; i < neighborCount; i++) {
    if (memcmp(neighbors[i].mac, msg.senderMac, 6) == 0) {
      neighbors[i].lastSeen = millis();
      neighbors[i].isRoot = msg.isRoot;
      neighbors[i].universe = msg.universe;
      neighbors[i].startChannel = msg.startChannel;
      neighbors[i].ledCount = msg.ledCount;
      found = true;
      break;
    }
  }
  if (!found && neighborCount < MAX_NEIGHBORS) {
    memcpy(neighbors[neighborCount].mac, msg.senderMac, 6);
    neighbors[neighborCount].lastSeen = millis();
    neighbors[neighborCount].isRoot = msg.isRoot;
    neighbors[neighborCount].universe = msg.universe;
    neighbors[neighborCount].startChannel = msg.startChannel;
    neighbors[neighborCount].ledCount = msg.ledCount;
    neighborCount++;
  }

  // Handle root heartbeat
  if (msg.isRoot) {
    rootLastSeen = millis();
    memcpy(rootMac, msg.senderMac, 6);

    // If I'm root and another node with lower MAC claims root, relinquish
    if (settings.isRoot && macLessThan(msg.senderMac, selfMac)) {
      Serial.println("Another node with lower MAC claims root, stepping down.");
      settings.isRoot = false;
      saveSettings();
    }
  }
}

void sendHeartbeat() {
  MeshMessage msg;
  msg.type = MSG_HEARTBEAT;
  memcpy(msg.senderMac, selfMac, 6);
  msg.isRoot = settings.isRoot;
  msg.universe = settings.universe;
  msg.startChannel = settings.startChannel;
  msg.ledCount = settings.ledCount;

  esp_err_t result = esp_now_send(NULL, (uint8_t *)&msg, sizeof(msg));
  if (result == ESP_OK) {
    Serial.print("Heartbeat sent as ");
    Serial.println(settings.isRoot ? "ROOT" : "NODE");
  } else {
    Serial.println("Error sending heartbeat");
  }
}

void setupWebServer() {
  // ... keep your previous web server setup here ...
  server.begin();
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  connectToWiFi();

  loadSettings();

  strip.updateLength(settings.ledCount);
  strip.begin();
  strip.show();

  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_wifi_get_mac(WIFI_IF_STA, selfMac);
  esp_now_register_recv_cb(onDataRecv);

  setupWebServer();

  if (inAPMode) {
    Serial.println("WebUI: http://192.168.4.1");
  } else {
    Serial.print("WebUI: http://");
    Serial.println(WiFi.localIP());
  }
}

unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 3000; // 3s
const unsigned long rootTimeout = 15000; // 15s root heartbeat timeout

void loop() {
  unsigned long now = millis();

  if (now - lastHeartbeat > heartbeatInterval) {
    sendHeartbeat();
    lastHeartbeat = now;
  }

  // Remove stale neighbors
  for (int i = 0; i < neighborCount;) {
    if (now - neighbors[i].lastSeen > 10000) {
      Serial.print("Removing stale neighbor: ");
      for (int j=0; j<6; j++) Serial.printf("%02X", neighbors[i].mac[j]);
      Serial.println();
      for (int k = i; k < neighborCount -1; k++) {
        neighbors[k] = neighbors[k+1];
      }
      neighborCount--;
    } else {
      i++;
    }
  }

  // Root failover logic:
  if (!settings.isRoot) {
    if (now - rootLastSeen > rootTimeout) {
      Serial.println("Root heartbeat lost, starting election...");
      // Elect self root if has lowest MAC among neighbors (or no neighbors)
      bool canBeRoot = true;
      for (int i = 0; i < neighborCount; i++) {
        if (macLessThan(neighbors[i].mac, selfMac)) {
          canBeRoot = false;
          break;
        }
      }
      if (canBeRoot) {
        Serial.println("Becoming new root!");
        settings.isRoot = true;
        saveSettings();
        memcpy(rootMac, selfMac, 6);
        rootLastSeen = now;
      }
    }
  }

  delay(50);
}
