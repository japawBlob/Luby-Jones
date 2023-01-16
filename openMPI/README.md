# OpenMP + OpenMPI

In this folder the implementations is expanded, to be able to be used with OpenMP and OpenMPI

For running and compiling is used enclosed Makefile. For each implementation: 
Serial, Parallel with threads, OpenMP, OpenMP+OpenMPI is created single file named
serial, parallel, openMP, openMPI respectively. All files can be build by using command
```make```. For building and running use ```make run```.

File for graph input can be specified in Makefile by setting ```INPUT_FILE``` variable, 
or by passing file path as program argument. Program argument is dominant over defined 
```INPUT_FILE```.

## Measurements

File used for benchmarking was 50000n-0.06p.in. Benchmarking was done on school computers with i5-7500 processor.

### OpenMP
```
iteration number: 0    Needed 3753 ms to finish OpenMP.
iteration number: 1    Needed 3758 ms to finish OpenMP.
iteration number: 2    Needed 3758 ms to finish OpenMP.
iteration number: 3    Needed 3753 ms to finish OpenMP.
iteration number: 4    Needed 3741 ms to finish OpenMP.
Average time needed for OpenMP compute: 3752ms. Number of colours: 5365
```

### OpenMPI
running on 4 computers:
MPI run command: 
```OMP_NUM_THREADS=4 mpirun --mca plm_rsh_args "-oStrictHostKeyChecking=no" --host 192.168.223.189,192.168.223.196,192.168.223.197,192.168.223.190 -np 4 -x OMP_NUM_THREADS ./openMPI```

```
iteration number: 0    Needed 34973 ms to finish OpenMPI.
iteration number: 1    Needed 19771 ms to finish OpenMPI.
iteration number: 2    Needed 19751 ms to finish OpenMPI.
iteration number: 3    Needed 19713 ms to finish OpenMPI.
iteration number: 4    Needed 20012 ms to finish OpenMPI.
Average time needed for OpenMPI compute: 22844ms. Number of colours: 5365
```
running on 2 computers
MPI run command: 
```OMP_NUM_THREADS=4 mpirun --mca plm_rsh_args "-oStrictHostKeyChecking=no" --host 192.168.223.189,192.168.223.196 -np 2 -x OMP_NUM_THREADS ./openMPI```
```
iteration number: 0    Needed 21164 ms to finish OpenMPI.
iteration number: 1    Needed 20974 ms to finish OpenMPI.
iteration number: 2    Needed 21191 ms to finish OpenMPI.
iteration number: 3    Needed 21174 ms to finish OpenMPI.
iteration number: 4    Needed 21127 ms to finish OpenMPI.
Average time needed for OpenMPI compute: 21126ms. Number of colours: 5365
```
running on 1 computer
MPI run command: 
```OMP_NUM_THREADS=2 mpirun --mca plm_rsh_args "-oStrictHostKeyChecking=no" --host 192.168.223.196,192.168.223.196 -np 2 -x OMP_NUM_THREADS ./openMPI```
```
iteration number: 0    Needed 8280 ms to finish OpenMPI.
iteration number: 1    Needed 8257 ms to finish OpenMPI.
iteration number: 2    Needed 8247 ms to finish OpenMPI.
iteration number: 3    Needed 8259 ms to finish OpenMPI.
iteration number: 4    Needed 8249 ms to finish OpenMPI.
Average time needed for OpenMPI compute: 8258ms. Number of colours: 5365
```

running on 1 computer
MPI run command: 
```OMP_NUM_THREADS=1 mpirun --host localhost,localhost,localhost,localhost -np 4 -x OMP_NUM_THREADS ./openMPI```
```
iteration number: 0    Needed 5512 ms to finish OpenMPI.
iteration number: 1    Needed 5514 ms to finish OpenMPI.
iteration number: 2    Needed 5518 ms to finish OpenMPI.
iteration number: 3    Needed 5534 ms to finish OpenMPI.
iteration number: 4    Needed 5516 ms to finish OpenMPI.
```

### std::thread

```
iteration number: 0    Needed 4138 ms to finish std::thread.
iteration number: 1    Needed 4126 ms to finish std::thread.
iteration number: 2    Needed 4101 ms to finish std::thread.
iteration number: 3    Needed 4040 ms to finish std::thread.
iteration number: 4    Needed 4041 ms to finish std::thread.
Average time needed for std::thread compute: 4089ms. Number of colours: 5365
```

### serial

```
iteration number: 0    Needed 12505 ms to finish Serial.
iteration number: 1    Needed 12517 ms to finish Serial.
iteration number: 2    Needed 12523 ms to finish Serial.
iteration number: 3    Needed 12708 ms to finish Serial.
iteration number: 4    Needed 12555 ms to finish Serial.
Average time needed for Serial compute: 12561ms. Number of colours: 5365
