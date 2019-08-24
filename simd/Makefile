.SUFFIXES: .cpp .o

CC = g++ 
CFLAGS = -g -Wall -march=athlon-xp -msse -DSIMD_USE_SSE
LDFLAGS = -lcppunit

%.o: %.cpp *.h
	$(CC) $(CFLAGS) -c $< -o $@


all: simdtest

simdtest: simdtest.o

clean: 
	rm *.o simdtest
