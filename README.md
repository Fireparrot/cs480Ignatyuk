# PA3: Moon

All files present are for PA3.
Those that remain from previous projects have not been changed, thus may remain as saying that they are for those previous projects, but they are nonetheless part of PA3.

# Building

In the folder containing the code (such as the include and src folders):
0.) rm build -r
1.) mkdir build
2.) cd build
3.) cmake ..
4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt.
Simply type in "./PA3".

# Libraries

No libraries other than the standard -- GLM, GLEW and SDL -- are needed.

# Controls

## Keyboard

Left key: make Planet cube orbit counterclockwise.
Right key: make Planet cube orbit clockwise.
Up key: make Planet cube orbit the other way (if orbitting).
Down key: make Planet cube not orbit.

## Mouse

Left button: if Planet cube is already rotating counterclockwise, stop; otherwise, rotate counterclockwise.
Right button: if Planet cube is already rotating clockwise, stop; otherwise, rotate clockwise.

