find_library(
	BtM_LIBRARY
	NAMES libLinearMath.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(BtM_LIBRARY)
	message(STATUS "Bullet Linear Math library loaded")
endif(BtM_LIBRARY)
    
    
if(BtM_LIBRARY)
	SET(BtM_FOUND TRUE)
ENDIF(BtM_LIBRARY)

if(BtM_FOUND)
	if(NOT BtM_FIND_QUIETLY)
		message(STATUS "Found Bullet Linear Math library: ${BtM_LIBRARY}")
	endif(NOT BtM_FIND_QUIETLY)
else(BtM_FOUND)
	if(BtM_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find Bullet Linear Math library")
	endif(BtM_FIND_REQUIRED)
endif(BtM_FOUND)
