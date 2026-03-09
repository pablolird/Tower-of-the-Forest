<p align="center">
  <img width="2430" height="1604" alt="image" src="https://github.com/user-attachments/assets/f105dfc8-46b7-4696-bb84-b7bd14e718d5" />
</p>

![GitHub Created At](https://img.shields.io/github/created-at/pablolird/Tower-of-the-Forest)
![GitHub contributors](https://img.shields.io/github/contributors/pablolird/Tower-of-the-Forest)

---

![C++ Badge](https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=fff&style=for-the-badge)
![SFML Badge](https://img.shields.io/badge/SFML-8CC445?logo=sfml&logoColor=fff&style=for-the-badge)
![Visual Studio Badge](https://img.shields.io/badge/Visual%20Studio-5C2D91?logo=visualstudio&logoColor=fff&style=for-the-badge)

# 🌲 Tower of the Forest — Tower Defense

**Tower of the Forest** is a 2D tower defense game built in C++ using SFML. Place and upgrade towers, set barricades, unleash powerful attacks, and survive increasingly relentless waves of forest creatures — goblins, wolves, bees, and slimes — through a dynamic day/night cycle.

---



https://github.com/user-attachments/assets/f0f318f4-dc13-45cb-a4fa-483072e47ab1



## 🌟 Features

- **Multiple Tower Types**: Place and upgrade three distinct towers — Target, Area, and Freeze — each with unique attack styles and upgrade paths.
- **Special Attacks**: Deploy powerful limited-use attacks like Lightning Strikes and Ice/Wood Spikes to turn the tide.
- **Barricades**: Place defensive structures to block and slow down enemy advances.
- **Enemy Variety**: Face four enemy types — Goblins, Wolves, Bees, and Slimes — each with their own animations and behavior.
- **Day/Night Cycle**: A dynamic lighting system shifts the atmosphere as waves progress.
- **Shop System**: Spend coins earned from defeating enemies to purchase towers and upgrades.
- **Upgrade System**: Evolve your towers through multiple upgrade levels to boost their effectiveness.
- **Tutorial**: An in-game multi-slide tutorial to get new players up to speed.
- **Settings & Credits**: Dedicated scenes for audio settings and team credits.
- **Sound Design**: Background music and sound effects for menus, gameplay, and interactions.
- **Entity-Component-System Architecture**: Clean, data-driven ECS design powering all game entities.
- **Quadtree Collision Detection**: Optimized spatial partitioning for efficient collision checks.

---

## 🗺️ Scenes

### ⚔️ Gameplay

https://github.com/user-attachments/assets/df628d75-ec2b-4f04-9a59-4637eb9af377

### 📖 Tutorial

<img width="1792" height="896" alt="tuto4" src="https://github.com/user-attachments/assets/01fe5d9d-74d2-49b1-ac61-0dcfaff6dc8f" />

---

## 🧟 Enemy Types

| Enemy  | Movement | Notes                     |
| ------ | -------- | ------------------------- |
| Goblin | Walking  | Basic melee attacker      |
| Wolf   | Walking  | Fast and aggressive       |
| Bee    | Flying   | Bypasses ground obstacles |
| Slime  | Walking  | Slow but resilient        |

---

## 🏰 Tower Types

| Tower        | Attack Style         | Upgrade Levels |
| ------------ | -------------------- | -------------- |
| Target Tower | Single-target archer | Up to 4        |
| Area Tower   | Multi-target archer  | Up to 4        |
| Freeze Tower | Slows enemies        | Up to 4        |

---

## 🛠️ Technologies Used

- **Language**: C++
- **Graphics & Windowing**: SFML (Simple and Fast Multimedia Library)
- **IDE**: Visual Studio
- **Architecture**: Entity-Component-System (ECS)
- **Collision**: Quadtree spatial partitioning
- **Asset Loading**: Custom `assets.txt` manifest

---

## 📂 Project Structure

```
Tower-of-the-Forest/
│
├── Main.cpp                   # Entry point
├── GameEngine.cpp/.h          # Core game loop and scene management
├── Scene.cpp/.h               # Base scene class
├── Scene_Play.cpp/.h          # Main gameplay scene
├── Scene_Menu.cpp/.h          # Main menu scene
├── Scene_Settings.cpp/.h      # Settings scene
├── Scene_Credits.cpp/.h       # Credits scene
│
├── Entity.cpp/.h              # ECS entity
├── EntityManager.cpp/.h       # Entity lifecycle management
├── Components.h               # All ECS components (CTransform, CHealth, etc.)
├── Animation.cpp/.h           # Sprite animation system
├── Physics.cpp/.h             # Collision detection
├── Quadtree.cpp/.h            # Quadtree spatial partitioning
├── Action.cpp/.h              # Input action abstraction
├── Vec2.cpp/.h                # 2D vector math
│
├── Assets.cpp/.h              # Asset manager (textures, fonts, sounds)
├── assets.txt                 # Asset manifest (fonts, textures, animations, music)
│
├── Assets/                    # Game assets
│   ├── Enemies/               # Goblin, Wolf, Bee, Slime sprites
│   ├── Tower/                 # Main tower idle & upgrade frames
│   ├── Attack/                # Archer towers & special attack sprites
│   ├── Defense/               # Barricade sprites
│   ├── Shop/                  # Shop UI icons
│   ├── Tutorial/              # Tutorial slide images
│   ├── Fonts/                 # Retro bitmap font
│   ├── Musics/                # Background music tracks
│   └── SoundEffects/          # UI sound effects
│
├── level1.txt                 # Level layout definitions
├── level2.txt
└── level3.txt
```

---

## 🚀 Setup and Installation

### Prerequisites

- [SFML 2.x](https://www.sfml-dev.org/download.php) installed and linked
- Visual Studio (Windows) or a C++17-compatible compiler with SFML configured

### Steps

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/pablolird/Tower-of-the-Forest.git
   cd Tower-of-the-Forest
   ```

2. **Open the Project:**
   - Open `Tower Defense - DS Project.vcxproj` in Visual Studio.
   - Ensure SFML include and library paths are configured in the project properties.

3. **Build and Run:**
   - Set the build configuration to **Release** or **Debug**.
   - Build the solution (`Ctrl+Shift+B`) and run (`F5`).
   - The executable will look for `assets.txt` in the working directory to load all game assets.

---

## 🎮 How to Play

### Controls

| Action          | Key / Input            |
| --------------- | ---------------------- |
| Navigate Menu   | `W` / `S` or `↑` / `↓` |
| Confirm / Enter | `Enter`                |
| Place / Select  | Left Click             |
| Cancel / Back   | Right Click / `Esc`    |
| Pause           | `P`                    |
| Restart         | `R`                    |
| Next (Tutorial) | `N`                    |
| Toggle Textures | `T`                    |
| Toggle Hitboxes | `C`                    |
| Toggle Info     | `H`                    |

### Gameplay Loop

1. **Start a wave** — enemies spawn and march toward your base along the road.
2. **Spend coins** from the shop to place towers on grass tiles.
3. **Upgrade towers** to increase their damage, range, and fire rate.
4. **Set barricades** on road tiles to obstruct enemy movement.
5. **Use special attacks** (Lightning, Ice Spikes, Wood Spikes) at critical moments.
6. **Survive** increasingly difficult waves across multiple levels.

---

## 👥 Team

- Pablo Lird
- Fernando Rojas
- Aristides Gernhofer
- Alvaro Lial
- Tamara Barrios

---

_Built as a Data Structures course final project._
