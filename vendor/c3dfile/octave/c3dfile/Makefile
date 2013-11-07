C3DSOURCE=../../src/c3dfile.cc \
					../../src/c3dutils.cc
C3DHEADER=../../src/c3dfile.h \
					../../src/c3dtypes.h \
					../../src/c3dutils.h 

CFLAGS=-g -I../../src
LDFLAGS=

c3dfile.oct: src/c3dfile.cc ${C3DSOURCE} ${C3DHEADER}
	mkoctfile ${CFLAGS} -o c3d_init.oct src/c3dfile.cc ${C3DSOURCE}
