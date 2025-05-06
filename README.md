# 🌟 Super Mario SFML Game

A simple 2D **Super Mario-style** platformer built with **C++** and **SFML**, recreating core gameplay features from the classic game including jumping, obstacles, coins, and a finish line. This project focuses on delivering a fun, smooth experience with an extendable structure for future levels and mechanics.

---

## 🎮 Features

- ✅ **Mario Movement**: Smooth walking and jumping mechanics with gravity simulation.
- 🧱 **Obstacles**:
  - Red blocks: Safe to land on or jump over.
  - Black blocks: Dangerous and trigger game over.
- 🪙 **Coins**: Collect coins throughout the level to fill a progress bar.
- 🏁 **Finish Line**: A special goal image indicates the end of the map.
- 🖼️ **Background Map**: Full image-based map for immersive visuals instead of tile-based rendering.
- 🌄 **Moving Sun & Camera**: Sun moves across the sky, and the camera follows Mario's progress.
- 🔁 **Restart Mechanism**: Press `R` after winning or losing to restart the level.
- 📏 **Extended Map Length**: Longer maps make gameplay more engaging and less predictable.
- ⛔ **Clean Finish Zone**: No obstacles near the finish line to allow fair completion.

---

## 🛠️ Setup Instructions

### 1. Requirements

- C++17 or higher
- SFML 2.5 or higher

### 2. Installation & Build

```bash
git clone https://github.com/yourusername/super-mario-sfml.git
cd super-mario-sfml
