# PA7: Solar System

Nikita Ignatyk and Roniel Padua

# Description

All files present are for PA7: Solar System.

# Building

In the folder containing the include, src, shaders and objects folders:

0.) rm build -r

1.) mkdir build

2.) cd build

3.) cmake ..

4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.

Simply type in "./PA7".

# Libraries

GLM, GLEW, SDL, Assimp and devIL are required libraries.

Assimp must be installed using "sudo apt-get install libassimp-dev". This should place Assimp .h/.hpp files in /usr/include/assimp/ and libassimp.so in /usr/lib/x86_64-linux-gnu/

devIL (outdated name: openIL) must be installed using "sudo apt-get install libdevil-dev". Its files should be installed in the same places as Assimp.

# Controls

## Keyboard

The up/down/left/right arrow keys change the angle from which the camera looks.

The j and k keys cycle between the nine planets to choose which one to focus on.

The numpad + and - keys speed up and slow down the passage of time, respectively.

The numpad * and / keys increase and decrease the scale of the distances between the sun and the planets (and the satellites around the planets, to a lesser degree).

## Mouse

Moving the mouse changes the angle of the camera, like the arrow keys do.

The scroll wheel zooms in and out.

## EXTRA CREDIT / ADDITIONS

Configuration file.

Live simulation speed adjustment.

All gas planets have (wonky) rings.

The scale of the distances between planets can be changed on the fly.

Earth has clouds that travel over the surface, and lights that turn on at night and off at day.

Satellites receive shadows from their parent planet.

