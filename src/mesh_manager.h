#pragma once
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_mesh.h"
#include "esp_event.h"

#define MESH_ID {0x7C, 0xDF, 0xA1, 0x60, 0x00, 0x01}
#define MESH_CHANNEL 6
#define MESH_MAX_LAYER 6
#define MESH_ROUTER_SSID "YourRouterSSID"
#define MESH_ROUTER_PASS "YourRouterPassword"

mesh_cfg_t mesh_config;
mesh_addr_t mesh_parent_addr;
int mesh_layer = -1;
bool is_mesh_root = false;
bool mesh_connected = false;

void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data) {
  switch (event_id) {
    case MESH_EVENT_STARTED:
      Serial.println("[MESH] Mesh started");
      break;
    case MESH_EVENT_PARENT_CONNECTED: {
      mesh_event_connected_t *connected = (mesh_event_connected_t *)event_data;
      memcpy(&mesh_parent_addr, connected->connected.bssid, 6);
      mesh_layer = connected->connected.layer;
      is_mesh_root = esp_mesh_is_root();
      mesh_connected = true;
      Serial.printf("[MESH] Connected to parent, layer %d, isRoot=%s\n",
                    mesh_layer, is_mesh_root ? "true" : "false");
      break;
    }
    case MESH_EVENT_PARENT_DISCONNECTED:
      Serial.println("[MESH] Disconnected from parent");
      mesh_connected = false;
      mesh_layer = -1;
      break;
    case MESH_EVENT_STOPPED:
      Serial.println("[MESH] Mesh stopped");
      break;
  }
}

void init_mesh(bool forceRoot) {
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.disconnect();

  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(esp_netif_init());

  mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
  uint8_t mesh_id[6] = MESH_ID;
  memcpy((uint8_t *)&cfg.mesh_id, mesh_id, 6);
  cfg.channel = MESH_CHANNEL;
  cfg.router.ssid_len = strlen(MESH_ROUTER_SSID);
  memcpy(&cfg.router.ssid, MESH_ROUTER_SSID, cfg.router.ssid_len);
  memcpy(&cfg.router.password, MESH_ROUTER_PASS, strlen(MESH_ROUTER_PASS));
  cfg.mesh_ap.max_connection = 6;
  cfg.mesh_ap.nonmesh_max_connection = 2;
  cfg.mesh_ap.authmode = WIFI_AUTH_WPA2_PSK;
  memcpy(cfg.mesh_ap.password, "meshpassword", strlen("meshpassword"));

  ESP_ERROR_CHECK(esp_mesh_init());
  ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL));
  ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

  if (forceRoot) {
    ESP_ERROR_CHECK(esp_mesh_set_self_organized(false, false));
    mesh_cfg_t new_cfg;
    esp_mesh_get_config(&new_cfg);
    new_cfg.mesh_ap.max_connection = 6;
    ESP_ERROR_CHECK(esp_mesh_set_config(&new_cfg));
    ESP_ERROR_CHECK(esp_mesh_fix_root(true));
  } else {
    ESP_ERROR_CHECK(esp_mesh_set_self_organized(true, true));
  }

  ESP_ERROR_CHECK(esp_mesh_start());
  Serial.println("[MESH] Mesh started");
}
