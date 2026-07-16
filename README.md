# ETS2 Damage Offset Dumper
**by prox0959**

A tool that scans Euro Truck Simulator 2's memory and automatically finds damage-related offsets for vehicle components.

---

## What Does It Do?

When launched while the game is running, it detects and prints the following 5 damage-related memory offsets:

| Component | Description |
|-----------|-------------|
| **Chassis** | Truck body damage offset |
| **Transmission** | Gearbox damage offset |
| **Engine** | Engine damage offset |
| **Cabin** | Cabin damage offset |
| **Wheel** | Tire/wheel wear offset |

---

## How To Use?

1. Open **Euro Truck Simulator 2** and load into a game.
2. Run `dumper.exe` **as Administrator**.
3. The program automatically attaches to the game, scans memory, and prints the offsets.

---

## Technical Details

- **Language:** C++ (Windows API)
- **Method:** AoB (Array of Bytes) / Pattern Scanning
- **Memory Access:** Read-only (`ReadProcessMemory`)
- **Target Process:** `eurotrucks2.exe`

---

## Requirements

- Windows 10/11
- Euro Truck Simulator 2
- Visual Studio 2019+ (for compilation)
