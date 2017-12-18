# PA11 Final: Computer Graphics Simulator

Nikita Ignatyk and Roniel Padua

# Description

All files present are for the final project, Computer Graphics Simulator

# Building

In the folder containing the include, src, shaders and objects folders:

Type and  run "./start.sh" in the terminal

**OR**

0.) rm build -r

1.) mkdir build

2.) cd build

3.) cmake ..

4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.

Simply type in "./cgs" if in the build directory, or "build/cgs" if right outside the build directory.

# Libraries

GLM, GLEW, SDL, Assimp, devIL, Bullet and freetype are required libraries.

Running start.sh will make sure they are installed, and if they are not, will prompt for sudo password to install them.

# Controls

## Keyboard

WASD = movement
QE = peek
F = flashlight
L = fragment/vertex lighting
T = throw ball!

## Mouse

motion = look around
LEFT = activate
RIGHT = (un)free mouse


