# PixelSurvivor 🎮

My first game project! A basic survival shooter where you defend against waves of enemies. This was created as a learning project to explore game development with Raylib.

## About the Game

In PixelSurvivor, you control a character at the bottom of the screen defending against falling enemies. Shoot them down before they reach you! As you progress through levels, more enemies appear and the challenge increases.

### Controls
- **Arrow Keys** or **A/D Keys**: Move left and right
- **Spacebar** or **Left Mouse Click**: Shoot
- **R Key**: Reload your weapon Or it will automatically reloads when magazine are empty
- **P** or **ESC**: Pause the game

## Credits

**Textures & Music**: by Dhanush U S - [GitHub Profile](GITHUB_ACCOUNT_LINK_HERE)

## For Developers

### What I Learned
This project was built to learn Raylib, a simple and easy-to-use game programming library written in C. Through this project, I got experience with:
- Game loop architecture
- Collision detection using circle-based hitboxes
- State management (Menu, Playing, Paused, Game Over)
- Asset loading and rendering
- Audio playback and sound effects
- Basic game mechanics (shooting, scoring, lives)

### Technical Stack
- **Language**: C
- **Library**: [Raylib](https://www.raylib.com/) - A simple and easy-to-use library to enjoy videogames programming
- **Tools**: MinGW (GCC), Make

### Building from Source

#### Prerequisites
- MinGW-w64 with GCC compiler ([Download here](https://www.mingw-w64.org/))
- Make sure `mingw32-make.exe` is in your system PATH

#### Build Instructions
1. Clone this repository
2. Open a terminal in the project directory
3. Run the build command:
   ```bash
   mingw32-make.exe PROJECT_NAME=PixelSurvivor OBJS=PixelSurvivor.c
   ```
4. Run the game:
   ```bash
   PixelSurvivor.exe
   ```

Alternatively, open the project in VS Code and press `Ctrl+Shift+B` to build.

### Project Structure
```
PixelSurvivor/
├── PixelSurvivor.c       # Main game code (~370 lines)
├── Makefile              # Build configuration
├── raylib-lib/           # Raylib library
│   └── src/
└── resources/            # Game assets
    ├── images/           # Character, enemy, bullet sprites
    └── audio/            # Sound effects and background music
```

### Development Insights
- **Game States**: Implemented using an enum to handle Menu, Playing, Paused, and GameOver states
- **Object Pooling**: Used fixed-size arrays for bullets and enemies
- **Collision Detection**: Simple circle-circle collision for bullet-enemy interactions
- **Scaling System**: Level progression scales enemy count based on kills (quadratic growth)

## License
See LICENSE file for details.

## Notes
- The game requires all resource files (images and audio) to be in the `resources/` folder
- If you move the raylib library, update the `RAYLIB_PATH` in the Makefile
