find_path(
    ftgl_INCLUDE_DIRS
	NAMES ftgl.h
	PATHS /usr/include/FTGL/
)

find_library(
	ftgl_LIBRARY
	NAMES libftgl.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(ftgl_INCLUDE_DIRS)
	message(STATUS "ftgl directories loaded")
endif(ftgl_INCLUDE_DIRS)
if(ftgl_LIBRARY)
	message(STATUS "ftgl library loaded")
endif(ftgl_LIBRARY)
    
    
if(ftgl_INCLUDE_DIRS AND ftgl_LIBRARY)
	SET(ftgl_FOUND TRUE)
ENDIF(ftgl_INCLUDE_DIRS AND ftgl_LIBRARY)

if(ftgl_FOUND)
	if(NOT ftgl_FIND_QUIETLY)
		message(STATUS "Found ftgl library: ${ftgl_LIBRARY}")
	endif(NOT ftgl_FIND_QUIETLY)
else(ftgl_FOUND)
	if(ftgl_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find ftgl library")
	endif(ftgl_FIND_REQUIRED)
endif(ftgl_FOUND)
