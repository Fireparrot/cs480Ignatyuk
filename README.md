# PA6: Model+Texture Loading

Nikita Ignatyk and Roniel Padua

# Description

All files present are for PA6.

The texture used is a map of the Earth. Two models calculate basic lighting while another does not.

# Building

In the folder containing the include, src, shaders and objects folders:

0.) rm build -r

1.) mkdir build

2.) cd build

3.) cmake ..

4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.

Simply type in "./PA6".

# Libraries

GLM, GLEW, SDL, Assimp and devIL are required libraries.

Assimp must be installed using "sudo apt-get install libassimp-dev". This should place Assimp .h/.hpp files in /usr/include/assimp/ and libassimp.so in /usr/lib/x86_64-linux-gnu/

devIL (outdated name: openIL) must be installed using "sudo apt-get install libdevil-dev". Its files should be installed in the same places as Assimp.

# Controls

These are here as a legacy, but they can be used to look at the "Planet" model from different angles and motions.

## Keyboard

Left key: make "Planet" orbit counterclockwise.

Right key: make "Planet" orbit clockwise.

Up key: make "Planet" orbit the other way (if orbitting).

Down key: make "Planet" not orbit.

## Mouse

Left button: if "Planet" is already rotating counterclockwise, stop; otherwise, rotate counterclockwise.

Right button: if "Planet" is already rotating clockwise, stop; otherwise, rotate clockwise.

