# Battleship

This is a Battleship game project developed by me and my friend. The game is implemented in C++ using the Qt framework.

## Demo

![Example](https://github.com/hoangledoan/Battleship/raw/main/battleship/ui/GUI/Img_file/battleship2.gif)

## Project Structure

The project is organized into several directories and files as follows:
```
battleship
├── build
├── logic
│ ├── game.cpp
│ └── game.hpp
├── net
│ ├── kinds.cpp
│ ├── kinds.h
│ ├── message.cpp
│ ├── message.h
│ ├── network.cpp
│ └── network.h
├── ui
├── docs
├── server
├── CMakeLists.txt
├── battleship.pro
├── battleship.pro.user
├── kabumm.cpp
├── kabumm.h
├── main.cpp
├── state.cpp
└── state.h
```
### Directories

- **build**: Contains the build files for the project.
- **logic**: Contains the main game logic.
  - `game.cpp`: Implementation of the game logic.
  - `game.hpp`: Header file for the game logic.
- **net**: Contains the networking code.
  - `kinds.cpp` / `kinds.h`: Implementation and header file for kinds.
  - `message.cpp` / `message.h`: Implementation and header file for message handling.
  - `network.cpp` / `network.h`: Implementation and header file for networking.
- **ui**: Directory intended for user interface code.
- **docs**: Documentation files.
- **server**: Server-side code.

### Files

- **CMakeLists.txt**: CMake configuration file.
- **battleship.pro**: Qt project file.
- **battleship.pro.user**: User-specific project settings.
- **kabumm.cpp** / **kabumm.h**: Additional game-related implementations.
- **main.cpp**: Main entry point of the application.
- **state.cpp** / **state.h**: Implementation and header file for managing game state.

## Building and Running

To build and run the project, you will need to have Qt and CMake installed.
