# PA4: Model Loading

All files present are for PA4.

Those that remain from previous projects have not been changed, thus may remain as saying that they are for those previous projects, but they are nonetheless part of PA4.

The models loaded are those of a cup and a bumpy sphere. The framework from PA3 was preserved, but the planet and moon are now bumpy spheres instead of cubes; there is an additional central cup.

I created the .obj files using a "dirty" program I wrote, therefore the files are not the best, but the important part is that they work -- and they load properly.


# Building

In the folder containing the code (such as the include and src folders):

0.) rm build -r

1.) mkdir build

2.) cd build

3.) cmake ..

4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.

Simply type in "./PA4".

# Libraries

No libraries other than the standard -- GLM, GLEW and SDL -- are needed.

# Controls

## Keyboard

Left key: make Planet orbit counterclockwise.

Right key: make Planet orbit clockwise.

Up key: make Planet orbit the other way (if orbitting).

Down key: make Planet not orbit.

## Mouse

Left button: if Planet is already rotating counterclockwise, stop; otherwise, rotate counterclockwise.

Right button: if Planet is already rotating clockwise, stop; otherwise, rotate clockwise.

