find_path(
    devILU_INCLUDE_DIRS
	NAMES ilu.h
	PATHS /usr/include/IL/
)

find_library(
	devILU_LIBRARY
	NAMES libILU.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(devILU_INCLUDE_DIRS)
	message(STATUS "devILU directories loaded")
endif(devILU_INCLUDE_DIRS)
if(devILU_LIBRARY)
	message(STATUS "devILU library loaded")
endif(devILU_LIBRARY)
    
    
if(devILU_INCLUDE_DIRS AND devILU_LIBRARY)
	SET(devILU_FOUND TRUE)
ENDIF(devILU_INCLUDE_DIRS AND devILU_LIBRARY)

if(devILU_FOUND)
	if(NOT devILU_FIND_QUIETLY)
		message(STATUS "Found devILU library: ${devILU_LIBRARY}")
	endif(NOT devILU_FIND_QUIETLY)
else(devILU_FOUND)
	if(devILU_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find devILU library")
	endif(devILU_FIND_REQUIRED)
endif(devILU_FOUND)
