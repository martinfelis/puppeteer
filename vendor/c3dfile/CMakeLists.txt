CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT( "c3dfile" CXX )

LIST ( APPEND CMAKE_MODULE_PATH
	${PROJECT_SOURCE_DIR}/CMake
	)	

# This line adds the directory PATH_TO/LIBRARY/include to our compiler include path.
INCLUDE_DIRECTORIES ("include/")

ADD_LIBRARY ( c3dfile STATIC
	src/c3dfile.cc
	src/c3dutils.cc
	)

ADD_EXECUTABLE ( c3dexport
	src/c3dexport.cc
)

SET_TARGET_PROPERTIES ( c3dfile PROPERTIES
	LINKER_LANGUAGE CXX
)

TARGET_LINK_LIBRARIES ( c3dexport
	c3dfile
	)

SUBDIRS (
	tests
	)
