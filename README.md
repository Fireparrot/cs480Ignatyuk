# PA5: Assimp Model Loading

Nikita Ignatyk and Roniel Padua

# Description

All files present are for PA5.

Those that remain from previous projects have not been changed, thus may remain as saying that they are for those previous projects, but they are nonetheless part of PA5.

The cup and bumpy sphere models were created by Nikita in a "quick and dirty" program, and the cube with holes was created by Nikita's brother in blender (sorry for offset of model's center).


# Building

In the folder containing the include, src, shaders and objects folders:

0.) rm build -r

1.) mkdir build

2.) cd build

3.) cmake ..

4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.

Simply type in "./PA5".

# Libraries

GLM, GLEW and SDL are required libraries.

Furthermore, Assimp (version 3.2.0) must be installed using "sudo apt-get install libassimp-dev". This should place Assimp .h/.hpp files in /usr/include/assimp/ and libassimp.so in /usr/lib/x86_64-linux-gnu/

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

