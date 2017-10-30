find_path(
    BtD_INCLUDE_DIRS
	NAMES btBulletDynamicsCommon.h
	PATHS /usr/include/bullet/
)

find_library(
	BtD_LIBRARY
	NAMES libBulletDynamics.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(BtD_INCLUDE_DIRS)
	message(STATUS "Bullet Dynamics directories loaded")
endif(BtD_INCLUDE_DIRS)
if(BtD_LIBRARY)
	message(STATUS "Bullet Dynamics library loaded")
endif(BtD_LIBRARY)
    
    
if(BtD_INCLUDE_DIRS AND BtD_LIBRARY)
	SET(BtD_FOUND TRUE)
ENDIF(BtD_INCLUDE_DIRS AND BtD_LIBRARY)

if(BtD_FOUND)
	if(NOT BtD_FIND_QUIETLY)
		message(STATUS "Found Bullet Dynamics library: ${BtD_LIBRARY}")
	endif(NOT BtD_FIND_QUIETLY)
else(BtD_FOUND)
	if(BtD_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find Bullet Dynamics library")
	endif(BtD_FIND_REQUIRED)
endif(BtD_FOUND)
