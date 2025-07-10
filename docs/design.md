# Mesh-Based DMX LED System

A powerful, modular lighting system using ESP32 devices with Wi-Fi mesh networking. Each node controls a WS2812 LED strip and optional DMX output (via MAX485). The system supports Art-Net input, WebUI configuration, and automatic mesh communication.

---

## ✨ Features
- Mesh network using ESP-MESH
- Art-Net input on root node
- LED output (WS2812) per node
- DMX output via MAX485
- WebUI for configuration
- NVS storage for persistent settings
- Node override and standalone operation
- Designed for future expansion

---

## 🚀 Getting Started

### 📦 Requirements
- ESP32-WROOM dev boards (one per node)
- WS2812B LED strips
- MAX485 transceiver chip (for DMX out)
- Optional: I2C OLED display and buttons

### 🔧 Setup
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/mesh-dmx-led.git
   ```
2. Open with PlatformIO or Arduino IDE
3. Configure Wi-Fi and settings in `config.h`
4. Upload to your ESP32 device

### 🌐 Access WebUI
- Connect node to Wi-Fi
- Access its IP via browser (auto-assigned via mDNS or mesh topology)

---

## 📁 Repo Structure
```
/firmware
  ├── src/               # Source code
  ├── data/              # Files for WebUI
  └── platformio.ini     # PlatformIO config
/docs
  ├── design.md          # Detailed system design
  └── mvp_plan.md        # MVP feature plan
README.md
LICENSE
```

---

## ⚙️ Development Status
Currently in MVP Phase:
- Mesh and Art-Net working
- LED and DMX output active
- WebUI config being expanded

Future goals include:
- Built-in effects and modes
- 2D editor for mesh layout
- OTA updates and mobile-first UI

---

## 📄 License
MIT License — see `LICENSE` file for details.

---

## 🤝 Contributing
Pull requests and feature suggestions are welcome. Join the development to help build the most modular and powerful DIY lighting control system!

