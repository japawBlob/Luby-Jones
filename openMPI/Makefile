#CC=mpicc # use this variantfor C language programs only
CC=mpiCC  # use variant for C++ programs to ensure linking with C++ libraries
CXX=mpiCC
#CC=arm-linux-gnueabihf-gcc
CFLAGS += -g -std=c99 -pedantic -Wall
CXXFLAGS += -g -std=gnu++11 -pedantic -Wall
#LDFLAGS += -lrt -lpthread

CFLAGS += -fopenmp
CXXFLAGS += -fopenmp
LDFLAGS += -fopenmp

CFLAGS += -lmpi
CXXFLAGS += -lmpi
LDFLAGS += -lmpi

all: main

main: main.o

.PHONY : depend dep all run

run: main
	OMP_NUM_THREADS=4 mpirun --host localhost,localhost,localhost -np 3 -x OMP_NUM_THREADS ./main

dep:
	$(CC) $(CFLAGS) $(CPPFLAGS) -w -E -M *.[Cc] \
	  > depend

depend:
	@touch depend

clean:
	rm -f *.o *.a main depend

-include depend