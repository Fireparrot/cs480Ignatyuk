#!/bin/bash

function setColor {
	printf "\e[48;5;$1m";
}
function resetColor {
	printf '\e[0m';
}
function echoColor {
	setColor $1
	printf "$2"
	resetColor
	echo ""
}
red=160
green=22
blue=20

installok="install ok installed"
check="dpkg-query -W -f=\${Status}"

function checkInstall {
	lib=$($check lib$1-dev)
	if [[ $lib = $installok ]]; then
		echoColor $green "$1 is installed"
	else
		echoColor $red "$1 is not installed!"
		sudo apt install lib$1-dev
	fi
}


checkInstall glew
checkInstall glm
checkInstall sdl2
checkInstall assimp
checkInstall devil
checkInstall bullet
checkInstall ftgl

if [ ! -d "build" ]; then
	echo "Making build directory"
	mkdir build
fi
cd build
cmake ..
make

program=$(find . -executable -type f -and -not -wholename './*/*')
program=${program:2}
echo ""
echoColor $blue "Ready to run!"
echo "Type \"build/$program\" into the terminal to run the program!"
resetColor
