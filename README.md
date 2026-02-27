# 🐉 Dragon Warriors  
### How to Train Your Dragon – Part II

---

## 📖 Project Overview

**Dragon Warriors** is a turn-based simulation project implemented in **C++**, inspired by the assignment  
**“How to Train Your Dragon – Part II”**.

The project simulates a battle on a 2D grid map between multiple warrior teams and a central boss — the **DragonLord**.  
All entities follow predefined movement rules, interact with obstacles, use items, and make strategic decisions over a fixed number of steps.

The implementation strictly follows:
- Object-Oriented Programming principles
- Manual memory management (no STL containers)
- The official assignment specification provided by HCMUT

---

## 🎯 Objectives

- Practice **Object-Oriented Programming (OOP)** in C++
- Apply **inheritance, polymorphism, and abstraction**
- Implement **dynamic memory management**
- Work with **file-based configuration input**
- Build a complete simulation system from scratch

---

## 🎮 Game Entities

### 🛩 FlyTeams
- **FlyTeam1** and **FlyTeam2**
- Move according to predefined movement rules (`L, R, U, D`)
- Can attack SmartDragons and defeat the DragonLord

### 🧱 GroundTeam
- Moves on the ground with its own movement rules
- Can **trap the DragonLord** for a number of turns
- Can pass **ground obstacles** if damage is sufficient

### 🐲 DragonLord
- Main boss of the simulation
- Moves based on the positions of FlyTeam1 and FlyTeam2
- Spawns **SmartDragons** periodically

### 🧠 SmartDragons
- **SD1, SD2, SD3**
- Spawn dynamically during the game
- Chase specific targets using Manhattan distance logic
- Drop items when defeated

---

## 🗺 Map & Environment

The game map is a 2D grid consisting of:

| Element Type        | Description |
|--------------------|-------------|
| `PATH`             | Walkable cell |
| `OBSTACLE`         | Completely blocked cell |
| `GROUND_OBSTACLE`  | Passable only if GroundTeam damage is high enough |

Each movement must be validated against the map constraints.

---

## 🎒 Items & Bag System

### Item Types
- **DragonScale** – increases damage
- **HealingHerb** – restores HP
- **TrapEnhancer** – enhances trap duration

### Bag System
- Each warrior owns a `BaseBag`
- Items are automatically used when possible
- `TeamBag` manages bags at team level

---

## 🔁 Game Mechanics

- Turn-based simulation
- Loop detection:
  - If an entity repeats movement patterns → eliminated
- SmartDragon creation every fixed number of DragonLord moves
- Interaction handling:
  - Warrior vs SmartDragon
  - GroundTeam vs DragonLord (trap)
  - FlyTeam vs DragonLord (win condition)

---

## ⚙️ Configuration File

The simulation is controlled by a **configuration file**, which defines:

- Map size
- Obstacles & ground obstacles
- Initial positions of all entities
- HP and damage values
- Movement rules
- Number of simulation steps

### Example:
```text
MAP_NUM_ROWS=10
MAP_NUM_COLS=10
NUM_OBSTACLE=2
ARRAY_OBSTACLE=[(1,2);(3,4)]
FLYTEAM1_MOVING_RULE=URDL
FLYTEAM1_INIT_POS=(0,0)
FLYTEAM1_INIT_HP=300
FLYTEAM1_INIT_DAMAGE=200
NUM_STEPS=100
```
---

## 🏗 Project Structure
```
├── dragon.h        # Class declarations
├── dragon.cpp      # Class implementations
├── main.cpp        # Provided driver (unchanged)
├── main.h          # Provided header (unchanged)
├── run.sh          # Build & run script
├── sa_tc_01_config # Sample configuration file
├── README.md       # Project documentation
└── .gitignore

```

---

## 🚀 How to Compile & Run

### Compile (Unix environment)

```bash
g++ -std=c++11 -Wall -Wextra main.cpp dragon.cpp -o dragon
```

### Run

```bash
./dragon sa_tc_01_config
```

⚠ The assignment is evaluated on Unix.




