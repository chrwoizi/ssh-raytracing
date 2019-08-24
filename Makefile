.SUFFIXES: .cpp .o

CC = g++

################################
# user settings

DEBUG = 0
OPTIMIZE = 1

# architecture type
# native: supported since g++-4.3. will enable all instruction subsets supported by the local machine (hence the result might not run on different machines)
#
ARCH = native
#ARCH = athlon64

################################
# automatic compiler flags configuration

# debug
# -g: enable debug
# -s: no symbols in executable
#
ifeq ($(DEBUG),1)
	DBG = -g
	SYM =
	DEFINE = _DEBUG
else
	DBG =
	SYM = -s
	DEFINE = NDEBUG
	
	# optimize
	#
	ifeq ($(OPTIMIZE),1)
		OPT = -O3
	else
		OPT =
	endif
endif

################################


CFLAGS = $(DBG) -Wall -ansi -pedantic -fopenmp $(OPT) -march=$(ARCH) -msse -m128bit-long-double -DSIMD_USE_SSE -U__DEPRECATED -D$(DEFINE) -Iply_utilities

LIBS = -L/usr/lib -L/usr/local/lib -lglut -lGLEW -lply
      
OBJECTS = simdtrace.o \
	RayTracer.o \
	Camera.o CameraController.o \
	Triangle.o \
	ModelParser.o ply_utilities/plyfile.o \
	kdTree.o kdSpatialMedianCut.o kdSurfaceAreaHeuristic.o \
	XHierarchy.o XHierarchySpatialMedianCut.o \
	Material.o \
	Image.o OpenGLTexture.o OpenGLDrawPixels.o PBO.o \
	bigfloat.o \
	
#PLYLoader.o
      
%.o: %.cpp *.h *.hpp
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

all: libply.a simdtrace

libply.a: ply_utilities/plyfile.c ply_utilities/ply.h
	$(CC) $(CFLAGS) -o ply_utilities/plyfile.o -c ply_utilities/plyfile.c
	ar cruv ply_utilities/libply.a ply_utilities/plyfile.o 
	-ranlib ply_utilities/libply.a

simdtrace: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(SYM) -o $@ $(OBJECTS)

clean: 
	rm *.o ply_utilities/*.o ply_utilities/*.a simdtrace

