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

// Mesh message types
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
  // Add more fields as needed
};

// --- Preferences for WiFi ---
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

void onDataRecv(const uint8_t * mac_addr, const uint8_t *data, int len) {
  if (len < sizeof(MeshMessage)) return; // ignore wrong packets
  MeshMessage msg;
  memcpy(&msg, data, sizeof(MeshMessage));

  // Debug output
  Serial.print("Received message type: ");
  Serial.println(msg.type);

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

  // Root selection logic:
  // If I am not root, and received root heartbeat, track root presence
  // If root disappears, become root

  // TODO: Implement full root election and failover logic here

}

void sendHeartbeat() {
  MeshMessage msg;
  msg.type = MSG_HEARTBEAT;
  memcpy(msg.senderMac, selfMac, 6);
  msg.isRoot = settings.isRoot;
  msg.universe = settings.universe;
  msg.startChannel = settings.startChannel;
  msg.ledCount = settings.ledCount;

  esp_err_t result = esp_now_send(NULL, (uint8_t *)&msg, sizeof(msg)); // broadcast
  if (result == ESP_OK) {
    Serial.println("Heartbeat sent");
  } else {
    Serial.println("Error sending heartbeat");
  }
}

void setupWebServer() {
  // ... Same as before, omitted for brevity ...
  // Add endpoints for /settings, /wifi, etc.
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

  // Init NeoPixel strip
  strip.updateLength(settings.ledCount);
  strip.begin();
  strip.show();

  // Setup ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_wifi_get_mac(WIFI_IF_STA, selfMac);
  esp_now_register_recv_cb(onDataRecv);

  setupWebServer();

  if (inAPMode) {
    Serial.println("WebUI: http://192.168.4.1");
  } else {
    Serial.println("WebUI: http://" + WiFi.localIP().toString());
  }
}

unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 3000; // every 3 seconds

void loop() {
  if (millis() - lastHeartbeat > heartbeatInterval) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }

  // Check neighbors timeout (remove old neighbors)
  unsigned long now = millis();
  for (int i = 0; i < neighborCount;) {
    if (now - neighbors[i].lastSeen > 10000) {
      Serial.print("Removing stale neighbor: ");
      for (int j=0; j<6; j++) Serial.printf("%02X", neighbors[i].mac[j]);
      Serial.println();
      // Remove by shifting
      for (int k = i; k < neighborCount -1; k++) {
        neighbors[k] = neighbors[k+1];
      }
      neighborCount--;
    } else {
      i++;
    }
  }

  // TODO: Root failover logic: if no root heartbeats for 15s, become root

  delay(50);
}
