# PA2: Spinning Controlled Cube

# Building

In the folder containing the code (such as the include and src folders):
0.) rm build -r
1.) mkdir build
2.) cd build
3.) cmake ..
4.) make

# Running

Running the project requires no command line arguments, nor does it give out a prompt. Simply type in "./Tutorial".

# Libraries

No libraries other than the standard -- GLM, GLEW and SDL -- are needed.

# Controls

## Keyboard

Left key: make cube orbit counterclockwise.
Right key: make cube orbit clockwise.
Up key: make cube orbit the other way (if orbitting).
Down key: make cube not orbit.

## Mouse

Left button: if already orbitting counterclockwise, stop; otherwise, orbit counterclockwise.
Right button: if already orbitting clockwise, stop; otherwise, orbit clockwise.

