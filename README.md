<p align="center">
  <img width="2430" height="1604" alt="Tower of the Forest title screen" src="https://github.com/user-attachments/assets/f105dfc8-46b7-4696-bb84-b7bd14e718d5" />
</p>

![GitHub Created At](https://img.shields.io/github/created-at/pablolird/Tower-of-the-Forest)
![GitHub contributors](https://img.shields.io/github/contributors/pablolird/Tower-of-the-Forest)

---

![C++ Badge](https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=fff&style=for-the-badge)
![SFML Badge](https://img.shields.io/badge/SFML-8CC445?logo=sfml&logoColor=fff&style=for-the-badge)
![Visual Studio Badge](https://img.shields.io/badge/Visual%20Studio-5C2D91?logo=visualstudio&logoColor=fff&style=for-the-badge)

# 🌲 Tower of the Forest — Tower Defense

**Tower of the Forest** is a 2D tower defense game written in C++ with SFML. Build and upgrade archer towers, block the roads with barricades and call down lightning while goblins, wolves, bees and slimes march on your tower from three directions. It runs on an Entity-Component-System engine, and a quadtree handles tower targeting and collision queries.

---

https://github.com/user-attachments/assets/f0f318f4-dc13-45cb-a4fa-483072e47ab1

## 🌟 Features

- **Three tower types:** Target (single-target archers), Area (wood spikes that hit every enemy on them) and Freeze (ice spikes that slow enemies). Each can be upgraded to level 3.
- **Barricades:** block the road until enemies break through. Bees fly over them.
- **Lightning strike:** an expensive shop item that damages everything on a chosen road tile.
- **Upgradable main tower:** up to level 4, each level adding 200 max health. A heal item restores 50 health.
- **Four enemy types**, with tougher boss variants once the waves peak.
- **Rising difficulty:** the spawn interval shrinks every second, from one enemy every 150 frames to one every 4 frames.
- **Day/night cycle**, drifting clouds, music and sound effects.
- **Menu, settings, credits**, pause and restart, and an 8-slide tutorial.

## ⚔️ Gameplay

https://github.com/user-attachments/assets/df628d75-ec2b-4f04-9a59-4637eb9af377

### 📖 Tutorial

<img width="1792" height="896" alt="Tutorial slide" src="https://github.com/user-attachments/assets/01fe5d9d-74d2-49b1-ac61-0dcfaff6dc8f" />

---

## 🎮 Controls

| Action | Input |
| --- | --- |
| Menu navigation | `W` / `S` or `↑` / `↓`, `Enter` |
| Buy an item / place it | Left click on the shop, then on a grass tile (towers) or road tile (barricades, lightning) |
| Cancel the selected item | Right click |
| Upgrade a tower or the main tower | Right click on it while hovering |
| Pause / help | `P` / `H` (`N` for the next tutorial slide) |
| Restart | `R` (when paused or defeated) |
| Back to menu | `Esc` |
| Debug: toggle textures / hitboxes | `T` / `C` |

---

## ⚡ Performance: fixing the quadtree

The game used a quadtree for tower range searches and collision checks, but it had a bug. `insert()` only split a node when that node already had children, so no node ever split. Every entity went into the root, and every "quadtree" query was really a linear scan. Once splitting worked, the original design would also have returned entities that cross a boundary more than once, split without limit when enemies piled up, and leaked child nodes.

The [rewrite](src/Quadtree.cpp):

- stores each entity once, in the deepest node that fully contains it,
- caps the depth,
- keeps off-screen enemies in the root, so queries still find them,
- returns results in the same order as the old scan, so towers pick the same targets as before.

Profiling then showed the 16 towers were going through about 37,000 query hits per frame. So towers now skip the search while they already have a target, and stop at the first enemy in range.

**In-game stress test.** Every grass and road slot is filled, and N invulnerable enemies walk the roads. Each run is 600 frames with vsync off, and the table shows the median of 3 runs. Every build ends with the same enemy-state checksum, so gameplay is unchanged.

| Enemies | Simulation time per frame, original → fixed | Frame rate, original → fixed |
| ---: | :---: | :---: |
| 1,000 | 0.83 → 0.34 ms (**2.4×**) | 430 → 568 FPS |
| 4,000 | 3.14 → 1.27 ms (**2.5×**) | 124 → 176 FPS |
| 8,000 | 6.63 → 2.65 ms (**2.5×**) | 61 → 89 FPS |
| 16,000 | 19.8 → 5.9 ms (**3.3×**) | 28 → 45 FPS |

Rendering one draw call per sprite is now most of the frame time, so frame rate improves less than simulation time.

**Headless query benchmark** (`make bench`, which rebuilds the index and runs the queries every frame):

| Workload | Entities | Original tree | Fixed tree | Speedup |
| --- | ---: | ---: | ---: | ---: |
| The game's queries (16 tower ranges, 16 collision boxes) | 5,000 | 2.77 ms | 1.32 ms | 2.1× |
| All-pairs collision (every entity queries its surroundings) | 5,000 | 339 ms | 33.6 ms | 10.1× |

Tower ranges cover about 18% of the map each, so the game's own queries return many hits and gain less than neighbour-sized queries do. Full data and the test machine are in [`src/bench/results/`](src/bench/results/).

`make test` runs randomized checks that every query returns exactly what a linear scan returns. It also covers subdivision, the depth limit, entities that cross boundaries and off-screen entities.

---

## 🛠️ Architecture

- **Entities and components:** an `Entity` holds one slot per component type in a `std::tuple` (`CTransform`, `CAnimation`, `CHealth`, `CRange`, …). Systems check `hasComponent<T>()` and read the data directly.
- **Entity lifetime:** `EntityManager` adds new entities at the start of the next frame and removes destroyed ones in one erase-remove pass. This keeps systems from modifying a list while looping over it. Entities are also indexed by tag (`enemy`, `archer`, `barricade`, …).
- **Spatial index:** after that update, `EntityManager` rebuilds the quadtree over sprite bounds once per frame. `queryRange()` serves tower targeting and collisions with the main tower and barricades.
- **Frame loop (`Scene_Play::update`):** entity update → health → collision → movement and targeting → spawning → placement → animation → info panel → upgrades → render.
- **Scenes and input:** `GameEngine` owns the window and the assets and switches between `Scene` subclasses (menu, play, settings, credits). Keys map to named actions, so scenes never read raw input.
- **Assets:** `assets.txt` lists every texture, animation, font, sound and music track, and `Assets` loads them at startup.

```
src/
├── Main.cpp · GameEngine.{h,cpp} · Scene.{h,cpp}
├── Scene_Play / Scene_Menu / Scene_Settings / Scene_Credits
├── EntityManager.{h,cpp}   # deferred add/remove, tag index, spatial queries
├── Entity.{h,cpp} · Components.h
├── Quadtree.{h,cpp}        # spatial index
├── Physics.{h,cpp}         # AABB overlap (current and previous frame)
├── Animation · Assets · Vec2 · Action
├── StressTest.h            # --stress benchmark mode
├── tests/test_quadtree.cpp
├── bench/                  # stress.sh, bench_quadtree.cpp, results/
├── Assets/ · assets.txt
└── Makefile · TowerOfTheForest.vcxproj · Doxyfile
```

---

## 🚀 Setup and Build

**Requirements:** a C++17 compiler and **SFML 2.6**. The code uses the SFML 2 API and does not compile against SFML 3.

### macOS / Linux

```bash
# macOS (Homebrew's default `sfml` is version 3, so use sfml@2)
brew install sfml@2
# Ubuntu/Debian
sudo apt install libsfml-dev

git clone https://github.com/pablolird/Tower-of-the-Forest.git
cd Tower-of-the-Forest/src
make run      # build and play
make test     # quadtree tests
make bench    # headless query benchmark
make stress   # in-game stress sweep (opens a window per run)
```

A single stress run: `./build/tower-of-the-forest --stress 8000 --frames 600 [--linear]`. To reproduce the "original" numbers, run `git checkout 19e3699` (the commit before the fix, which already has the stress harness) and run the same command.

### Windows (Visual Studio)

Open `TowerOfTheForest.sln` and set the SFML include and library paths in the project properties. Then build and run with `src/` as the working directory, so the game finds `assets.txt` and `Assets/`.

### Known issue

The window has a fixed size of 1792×896 and lays out the map from the window size. On displays smaller than that, the OS shrinks the window and the map layout breaks.

---

_Built by a team of five for a Data Structures course at UPTP (Universidad Politécnica Taiwán-Paraguay), Jun–Jul 2024._
