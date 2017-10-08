find_path(
    devILUT_INCLUDE_DIRS
	NAMES ilut.h
	PATHS /usr/include/IL/
)

find_library(
	devILUT_LIBRARY
	NAMES libILUT.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(devILUT_INCLUDE_DIRS)
	message(STATUS "devILUT directories loaded")
endif(devILUT_INCLUDE_DIRS)
if(devILUT_LIBRARY)
	message(STATUS "devILUT library loaded")
endif(devILUT_LIBRARY)
    
    
if(devILUT_INCLUDE_DIRS AND devILUT_LIBRARY)
	SET(devILUT_FOUND TRUE)
ENDIF(devILUT_INCLUDE_DIRS AND devILUT_LIBRARY)

if(devILUT_FOUND)
	if(NOT devILUT_FIND_QUIETLY)
		message(STATUS "Found devILUT library: ${devILUT_LIBRARY}")
	endif(NOT devILUT_FIND_QUIETLY)
else(devILUT_FOUND)
	if(devILUT_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find devILUT library")
	endif(devILUT_FIND_REQUIRED)
endif(devILUT_FOUND)
