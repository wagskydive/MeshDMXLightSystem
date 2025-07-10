# MVP Implementation Plan

## 🔨 Objective
Deliver a fully functional minimum viable product of the Mesh-Based DMX LED System.

## 🚩 Target Outcomes
- Art-Net input on root node
- Mesh network communication
- LED + DMX output per node
- WebUI for configuration
- Persistent NVS storage

---

## 📦 Phase 1 – Core Mesh + Communication
- [ ] Set up ESP-MESH with static root node
- [ ] Implement node discovery + sync messaging
- [ ] Define mesh broadcast packet format

## 🎛 Phase 2 – LED + DMX Output
- [ ] WS2812 LED strip output via GPIO 13
- [ ] Basic MAX485 DMX output (configurable universe/channel)
- [ ] Implement LED brightness limit
- [ ] NVS storage for:
  - LED count
  - Universe + DMX channel (LED + MAX485)
  - Max current

## 🌐 Phase 3 – WebUI Configuration
- [ ] AsyncWebServer serving simple HTML UI
- [ ] Node list display with editable settings
- [ ] Upload settings to NVS
- [ ] Optional: root node toggle

## 💡 Phase 4 – Config Feedback + Override
- [ ] LED visual feedback during config
- [ ] Node override mode
  - [ ] Fixed color
  - [ ] Local effects (static / wipe)
- [ ] Ignore mesh input if override enabled

## ✅ MVP Complete When:
- One or more nodes join a mesh
- Root receives Art-Net data
- All nodes light up LEDs according to Art-Net or local settings
- DMX output sends correct values
- WebUI available for config per node

