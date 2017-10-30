find_path(
    BtC_INCLUDE_DIRS
	NAMES btBulletCollisionCommon.h
	PATHS /usr/include/bullet/
)

find_library(
	BtC_LIBRARY
	NAMES libBulletCollision.so
	PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib/ /usr/lib/i386-linux-gnu/
)

if(BtC_INCLUDE_DIRS)
	message(STATUS "Bullet Collision directories loaded")
endif(BtC_INCLUDE_DIRS)
if(BtC_LIBRARY)
	message(STATUS "Bullet Collision library loaded")
endif(BtC_LIBRARY)
    
    
if(BtC_INCLUDE_DIRS AND BtC_LIBRARY)
	SET(BtC_FOUND TRUE)
ENDIF(BtC_INCLUDE_DIRS AND BtC_LIBRARY)

if(BtC_FOUND)
	if(NOT BtC_FIND_QUIETLY)
		message(STATUS "Found Bullet Collision library: ${BtC_LIBRARY}")
	endif(NOT BtC_FIND_QUIETLY)
else(BtC_FOUND)
	if(BtC_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find Bullet Collision library")
	endif(BtC_FIND_REQUIRED)
endif(BtC_FOUND)
