#cc=gcc
CC=g++
mpi=mpiCC

COMPILE_FLAGS= -O3 -std=c++14 -Wall -pipe -g -march=native
PTHREAD= -pthread
OPENMP= -fopenmp

thread_flags=-DTHREAD
openMP_flags=-DOPENMP
openMPI_flags=-DOPENMPI

PROGRAM_FILES=main.cpp
INPUT_FILE= -D'INPUT_FILE="../data_in/10000n-0.995p.in"'

all: serial parallel openMP openMPI

serial: main.cpp
	$(CC) $(COMPILE_FLAGS) $(PROGRAM_FILES) $(INPUT_FILE) -o serial

parallel: main.cpp
	$(CC) $(COMPILE_FLAGS) $(thread_flags) $(PTHREAD) $(PROGRAM_FILES) $(INPUT_FILE) -o parallel

openMP: main.cpp
	$(CC) $(COMPILE_FLAGS) $(openMP_flags) $(PTHREAD) $(OPENMP) $(PROGRAM_FILES) $(INPUT_FILE) -o openMP

openMPI: main.cpp
	$(mpi) $(COMPILE_FLAGS) $(openMPI_flags) $(PROGRAM_FILES) $(PTHREAD) $(OPENMP) $(INPUT_FILE) -o openMPI

run: all run_all

run_all:
	./serial
	./parallel
	./openMP
	OMP_NUM_THREADS=2 mpirun --host localhost,localhost,localhost,localhost -np 4 -x OMP_NUM_THREADS ./openMPI

run_MPI:
	OMP_NUM_THREADS=2 mpirun --host localhost,localhost,localhost,localhost -np 4 -x OMP_NUM_THREADS ./openMPI

clean:
	rm serial parallel openMP openMPI