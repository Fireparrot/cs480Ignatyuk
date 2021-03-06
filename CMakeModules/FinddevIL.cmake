find_path(
    devIL_INCLUDE_DIRS
	NAMES il.h
	PATHS /usr/include/IL/
)

find_library(
	devIL_LIBRARY
	NAMES libIL.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(devIL_INCLUDE_DIRS)
	message(STATUS "devIL directories loaded")
endif(devIL_INCLUDE_DIRS)
if(devIL_LIBRARY)
	message(STATUS "devIL library loaded")
endif(devIL_LIBRARY)
    
    
if(devIL_INCLUDE_DIRS AND devIL_LIBRARY)
	SET(devIL_FOUND TRUE)
ENDIF(devIL_INCLUDE_DIRS AND devIL_LIBRARY)

if(devIL_FOUND)
	if(NOT devIL_FIND_QUIETLY)
		message(STATUS "Found devIL library: ${devIL_LIBRARY}")
	endif(NOT devIL_FIND_QUIETLY)
else(devIL_FOUND)
	if(devIL_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find devIL library")
	endif(devIL_FIND_REQUIRED)
endif(devIL_FOUND)
