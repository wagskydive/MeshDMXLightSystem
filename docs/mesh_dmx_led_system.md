# 📘 Mesh-Based DMX LED Control System – Design Document

## Overview

This project is a robust, modular lighting control system built on ESP32 devices using a Wi-Fi mesh network. Each node can control WS2812 LED strips and output DMX data via MAX485. Nodes can operate in a mesh-controlled or standalone mode, and are configurable via an onboard WebUI or optional OLED interface.

---

## 🔑 Core Features

- ESP-MESH-based communication with automatic topology management
- Art-Net input on root node, broadcast over mesh
- Per-node LED output (WS2812 via GPIO)
- DMX output via MAX485
- Onboard WebUI for all configuration
- NVS for persistent per-node settings
- Node override mode (standalone effect or color)
- Future-proof for external DMX fixture control

---

## ✅ Minimum Viable Product (MVP)

### Features:

- Mesh network formation with manual root node
- Art-Net receiver on root node
- Mesh-wide pixel data broadcast
- LED output per node (GPIO 13 default)
- MAX485 DMX output support
- NVS memory for: LED count, DMX universe, channel, DMX out settings
- Basic WebUI for:
  - Setting node parameters
  - Choosing root node
  - Manual mode toggle
- Visual LED feedback during configuration

---

## 🧭 Implementation Plan

### Phase 1 – Mesh Stability

- Persistent node state and NVS backups
- Root node handoff handling
- Optional signal-strength-based root election

### Phase 2 – Full WebUI Config Hub

- Node discovery in WebUI
- Per-node configuration panel
- NVS sync with UI
- WebUI served from root node

### Phase 3 – Visual Topology + Node Override Mode

- 2D layout editor (basic implementation)
- Curve drawing for LED strip layout
- Node override mode:
  - Fixed color
  - Static effects
  - Full manual mode
- Mesh ignores overridden nodes

### Phase 4 – Effects Engine

- DMX fixture modes (3ch, 5ch, Pixel)
- Internal procedural effects engine
- Tap-tempo & BPM sync
- WebUI effect sliders

### Phase 5 – Scene Management

- Save/load scenes to NVS
- DMX or WebUI recall
- Named scene support

### Phase 6 – DMX Fixture Integration

- DMX profile editor in WebUI
- Art-Net + internal control for pan/tilt, color, etc.
- Treat DMX fixtures as virtual mesh nodes

### Phase 7 – OTA & Sync

- WebUI firmware update
- OTA propagation across mesh
- Clock/time sync via NTP

### Phase 8 – Mobile WebUI Polish

- Responsive layout
- Dark mode
- Touch-optimized controls (blackout, strobe)

### Phase 9 – Visual Output Preview

- 2D live simulation of mesh
- Debug & design support

### Phase 10 – Advanced Integration

- WebSocket + MIDI support
- MIDI-over-Wi-Fi
- External app control hooks

---

## 🧱 Technical Architecture

### Hardware

- ESP32-WROOM (per node)
- WS2812B LED strips (5V)
- MAX485 DMX transceiver
- I2C OLED (for future use)
- Pushbuttons for UI

### Software

- ESP-IDF / Arduino Core for ESP32
- AsyncWebServer for WebUI
- NVS for persistent storage
- FastLED or Adafruit\_NeoPixel for LED
- Art-Net library (root only)

### Mesh Network

- ESP-MESH with broadcast messaging
- Root node handles Art-Net and coordinates state
- Nodes listen for their configured universe/channel
- Override mode bypasses mesh input

---

## 📡 Node Modes

| Mode              | Description                             |
| ----------------- | --------------------------------------- |
| Mesh Controlled   | Normal operation (follows Art-Net/mesh) |
| Override - Color  | Fixed RGB set via WebUI                 |
| Override - Effect | Runs local effect, ignores mesh         |
| Manual Mode       | Full user control, no sync              |

---

## 📂 Directory Structure (for GitHub)

```
/firmware
  ├── src/
  ├── data/
  └── platformio.ini
/docs
  ├── design.md
  └── mvp_plan.md
README.md
LICENSE
```

---

## ✅ Ready for GitHub

This document will serve as `docs/design.md` in the repository. We are ready to generate:

- `README.md` (project summary, features, build instructions)
- `docs/mvp_plan.md` (condensed roadmap)
- Optionally: `docs/webui_mockups.md` with WebUI sketches

