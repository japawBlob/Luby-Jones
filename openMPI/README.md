# OpenMP + OpenMPI

In this folder the implementations is expanded, to be able to be used with OpenMP and OpenMPI

For running and compiling is used enclosed Makefile. For each implementation: 
Serial, Parallel with threads, OpenMP, OpenMP+OpenMPI is created single file named
serial, parallel, openMP, openMPI respectively. All files can be build by using command
```make```. For building and running use ```make run```.

File for graph input can be specified in Makefile by setting ```INPUT_FILE``` variable, 
or by passing file path as program argument. Program argument is dominant over defined 
```INPUT_FILE```.