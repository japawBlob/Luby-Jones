CC=mpicc # use this variantfor C language programs only
#CC=mpiCC  # use variant for C++ programs to ensure linking with C++ libraries
CXX=mpiCC
#CC=arm-linux-gnueabihf-gcc
CFLAGS += -g -std=c99 -pedantic -Wall
CXXFLAGS += -g -std=gnu++11 -pedantic -Wall 
#LDFLAGS += -lrt -lpthread

#CFLAGS += -fopenmp
#LDFLAGS += -fopenmp

CFLAGS += -lmpi
LDFLAGS += -lmpi

MPIRUN_OPTIONS += --mca plm_rsh_args "-oStrictHostKeyChecking=no"

all: mpi-test-1

program: mpi-test-1.o

.PHONY : depend dep all run

run: mpi-test-1
	mpirun $(MPIRUN_OPTIONS) --host localhost,localhost,localhost,localhost -np 4 ./mpi-test-1

dep:
	$(CC) $(CFLAGS) $(CPPFLAGS) -w -E -M *.[Cc] \
	  > depend

depend:
	@touch depend

clean:
	rm -f *.o *.a mpi-test-1 depend

-include depend
