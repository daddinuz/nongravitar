# Gravitar

Computer Science programming course project at University of Bologna 2018/2019.

## Requirements

To be able to build this project a full-featured compiler that supports at least **C++17** must be provided;  
moreover the following requirements must be satisfied:

 - [**CMake**](https://cmake.org) version 3.10 or later.
 - [**SFML**](https://www.sfml-dev.org/index.php) version 2.4 or later.

Furthermore this project is heavly based on [**EnTT**](https://github.com/skypjack/entt) which is already provided under the `/deps` folder.

**NOTE:** This project has been developed on a machine running **Ubuntu 18.04** so I've no clue if it's going to work on 
any other OS or even other Linux flavours, anyway I reasonably expect this to work on other **apt-based** distros.

Below a snippet to install the requirements on Ubuntu 18.04:

```bash
sudo apt update
sudo apt install build-essential
sudo apt install cmake
sudo apt install libsfml-dev
```

## Building

In order to build the project run on the root folder of the project:

```bash
mkdir -p cmake-build-release
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

now you should see the **gravitar** bin under `/cmake-build-release` folder.
You can finally run the game:

```bash
./gravitar
```

## How to play

The game will prompt the player (a Cyan spaceship) in a solar system with 8 different planets.
The player is free to navigate the universe, enter or leave the planets as he likes, but in order 
to win he must defeat all the enemies (bunkers) that lives on the planets.

When the player leaves a planet with no bunkers left, the planet will explode and will no longer be accessible.

**SpaceShip Controls:**

+ Keyboard:
  - **[A]**: anti-clockwise rotate.
  - **[W]**: move faster in the current direction.
  - **[D]**: clockwise rotate.
  - **[S]**: move slower in the current direction.
+ Mouse:
  - **[LeftClick]**: fire guns.
  - **[RightClick]**: activate tractor ray.

**Note:** pressing **[ESC]** will exit the game on any time without asking confirmation.

