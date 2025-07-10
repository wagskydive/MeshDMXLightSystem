# Mesh-Based DMX LED Control System

## Overview

A modular, mesh-capable lighting control firmware for ESP32-based devices. Designed for live environments, installations, and mobile rigs, this system enables wireless, synchronized LED and DMX control across a scalable mesh of nodes.

## Features

- 📶 **ESP-MESH networking** for robust, self-healing communication
- 🎨 **WS2812 LED strip output** with dynamic Art-Net pixel mapping
- 🎚 **DMX output via MAX485** (controllable via universe/channel config)
- 🌐 **WebUI** served from mesh root for full configuration
- 🧠 **NVS memory** for persistent per-device settings
- 🕹 Optional **OLED UI + buttons** for local config and debugging
- 🚦 **Visual feedback via LED strip** during configuration
- 🕹️ **Manual override mode** per node for effects or fixed color
- 🔧 Designed for future expansion: DMX fixtures, scenes, effects

## Hardware Requirements

- ESP32-WROOM-based boards (e.g., NodeMCU-32S)
- WS2812B-compatible LED strips
- MAX485 module (for DMX out)
- Optional: OLED display (I2C), pushbuttons, MOSFET display power cutoff

## Software Stack

- ESP-IDF / Arduino Core for ESP32
- ESP-MESH
- AsyncWebServer
- ArtNet receiver (root node only)
- FastLED or Adafruit NeoPixel

## Directory Structure

```
/firmware         - PlatformIO-based source code
  /src            - Main firmware files
  /data           - WebUI assets for SPIFFS
/docs             - Design and planning documents
  design.md       - Complete system architecture
  mvp_plan.md     - Build roadmap and phases
README.md         - This file
LICENSE           - MIT (default, change if needed)
```

## Getting Started

1. Clone this repository
2. Use [PlatformIO](https://platformio.org/) to upload `/firmware` to ESP32
3. Connect nodes to 5V and boot; root node will host WebUI
4. Access WebUI via local IP of root (displayed on serial)
5. Configure mesh and output settings

## Contributing

Contributions and collaboration are welcome. See `docs/design.md` for feature map and future plans.

## License

MIT

