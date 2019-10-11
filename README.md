# NonGravitar

Computer Science programming course project at University of Bologna 2018/2019.  
The project consists of implementing a game inspired by the Atari Gravitar released in the 1982.

## Requirements

To be able to build this project a full-featured compiler that supports at least **C++17** must be provided;  
moreover the following requirements must be satisfied:

- [CMake](https://cmake.org) version 3.10 or later.
- [SFML](https://www.sfml-dev.org/index.php) version 2.4 or later.

Furthermore this project is heavily based on [EnTT](https://github.com/skypjack/entt) which is already provided under the `/deps` folder.

**NOTE:** This project has been developed on a machine running **Ubuntu 18.04** so I've no clue if it's going to work on 
any other OS or even other Linux flavours, anyway I reasonably expect this to work on other **apt-based** distributions.

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

now you should see the **nongravitar** bin under `/cmake-build-release` folder.
You can finally run the game:

```bash
./nongravitar
```

## How to play

The game will prompt you (a Space Explorer) in a solar system with 8 different planets which 
are full of resources like energy or spare parts useful to repair you spaceship and continue your journey.

You are free to navigate the planets as you like and to collect resources using your 
spaceship's ray tractor, but be careful, the planets may be crowded by hostile bunkers 
that are going to shoot you at first sight, in order to protect the resources present on the planet.

In order to win, you must visit the whole solar system and defeat all the enemies that lives on the planets;
when you leave a planet with no bunkers left, the planet will explode and will no longer be accessible.

During your journey pay attention to your vehicle, if it gets too damaged or if you finish the energy your 
journey will end, and you'll be lost in space!

**SpaceShip Controls:**

+ Keyboard:
  - **[A]**: anti-clockwise rotate.
  - **[W]**: move faster in the current direction.
  - **[D]**: clockwise rotate.
  - **[S]**: move slower in the current direction.
+ Mouse:
  - **[LeftClick]**: shoot.
  - **[RightClick]**: activate tractor ray.

**Note:** pressing **[ESC]** will exit the game at any time without asking confirmation.

## Design decisions

#### Scene handling

I've implemented a rudimentary scene system using the [state pattern](https://en.wikipedia.org/wiki/State_pattern) for 
handling the various scenes interactions.

The game is composed by the following scenes:

- TitleScreen
- SolarSystem
- PlanetAssault
- GameOver
- YouWon

Each scene is autonomous and independent from the others, so I managed to split 
the complexity of each scene into smaller pieces that are easier to maintain and expand.

#### Memory management

Regarding memory management, I decided to take the simplest approach possible: allocate all the heavy and long-lived 
resources like textures, fonts or soundtracks at the program startup and deallocate all of them on exit.

In this way I got "whole-program lifetime" of the assets thus avoiding loading phases during the game and a throwaway 
usage of the assets which is a waste of time and memory, so by the point of view of the game, assets can be seen 
as a [singleton](https://en.wikipedia.org/wiki/Singleton_pattern) ensuring proper initialization and de-allocation.

#### About the game logic

Concerning the core logic of the game I used a [data driven](https://en.wikipedia.org/wiki/Data-driven_programming) 
approach using [ECS (Entity Component System)](https://en.wikipedia.org/wiki/Entity_component_system) provided by the 
[EnTT](https://github.com/skypjack/entt) library thus avoiding a complex hierarchy of inheritance of game objects.

## LICENSE

MIT license.  
I don't own any rights on the original Atari game, this project is intended for educational purposes only.

## CREDITS

Many thanks to:
- [Michele Caini](https://github.com/skypjack) for the [EnTT](https://github.com/skypjack/entt) framework. 
- [Drozerix](https://modarchive.org/index.php?request=view_profile&query=84702) for his amazing soundtracks.
