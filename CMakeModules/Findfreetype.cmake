find_path(
    freetype_INCLUDE_DIRS
	NAMES ft2build.h
	PATHS /usr/include/freetype2/
)

find_library(
	freetype_LIBRARY
	NAMES libfreetype.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(freetype_INCLUDE_DIRS)
	message(STATUS "freetype directories loaded")
endif(freetype_INCLUDE_DIRS)
if(freetype_LIBRARY)
	message(STATUS "freetype library loaded")
endif(freetype_LIBRARY)
    
    
if(freetype_INCLUDE_DIRS AND freetype_LIBRARY)
	SET(freetype_FOUND TRUE)
ENDIF(freetype_INCLUDE_DIRS AND freetype_LIBRARY)

if(freetype_FOUND)
	if(NOT freetype_FIND_QUIETLY)
		message(STATUS "Found freetype library: ${freetype_LIBRARY}")
	endif(NOT freetype_FIND_QUIETLY)
else(freetype_FOUND)
	if(freetype_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find freetype library")
	endif(freetype_FIND_REQUIRED)
endif(freetype_FOUND)
