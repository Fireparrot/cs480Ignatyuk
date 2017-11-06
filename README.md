# PA9: Bullet + Lighting

Nikita Ignatyk and Roniel Padua

# Description

All files present are for PA9: Bullet + Lighting.

# Building

In the folder containing the include, src, shaders and objects folders:

0.) rm build -r

1.) mkdir build

2.) cd build

3.) cmake ..

4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.

Simply type in "./PA9".

# Libraries

GLM, GLEW, SDL, Assimp, devIL and Bullet are required libraries.

Assimp must be installed using "sudo apt-get install libassimp-dev". This should place Assimp .h/.hpp files in /usr/include/assimp/ and libassimp.so in /usr/lib/x86_64-linux-gnu/

devIL (outdated name: openIL) must be installed using "sudo apt-get install libdevil-dev". Its files should be installed in the same places as Assimp.

Bullet should be installed using "sudo apt-get install libbullet-dev". This installs <i>both the includes and libraries</i> for Bullet. <b>This project depends on the includes and libraries for Bullet to be installed in the folders as above.</b>

# Controls

## Keyboard

The up/down/left/right arrow keys move the cube.

e changes between per-fragment and per-vertex lighting.
Keypad / and * decreases and increases (respectively) the spotlight's brightness.
Keypad - and + decreases and increases (respectively) the spotlight's "size".
r and f increases and decreases (respectively) all objects' ambient constant.
t and g increases and decreases (respectively) the ball's specular constant.
c switches between a cyan light and a white light for the spotlight.

## Mouse

Moving the mouse moves the cube. The mouse may be invisibly bounded by the window, preventing movement in certain directions after the mouse travels too far.

## NOTE

If you launch any of the two mobile objects off the board, it's your fault ;P
Restart the program in that case, if you want them back.

