# Luby Jones Algorytm

This repository is result of my work on Luby Jones algorythm, and it's parallelization.
For basic functionality with serial computation and basic threaded implementation use this folder and use included
cmake in order to run the program.

## What is Luby Jones Algorithm?

Luby-Jones is based on Luby's algorithm, which is used for finding Maximal independent set. Luby-Jones is based on
principle of finding independent sets, and iteratively colours graph.

## How to use this program?

Program accepts following arguments:
- ```-f [file_path]```, where file_path is input file. If left empty input will be taken from std::in. 
Input data has the following format: On first line are two unsigned integers A, B. Where A represents number of nodes and B 
represents number of edges. This line is followed by B lines, consisting of two unsigned integers C D, where C and D are
connected nodes. For inspiration look into graph_generator and data_in folders.
- ```-i [number_of_iterations]``` number of times the computation will be executed.
- ```-p``` if this flag is enabled, program will run in parallel. Without this flag program runs in single threaded mode.

Comprehensive parallelization implemented in openMPI folder.

This repository also contains graph generator written in python, that can be used to generate your own graphs. It is
located in graph_generator folder with its own readme.

After completing, program also generates .dot file in graphviz folder, which can be compiled to .png file using included
Makefile.

### Measurements:
Measurement were executed on commit b85d080.
Hardware: i7-4770 - 4 cores 8 threads. OS: Kubuntu 22.04.1 LTS x86_64.
Measurements were done on file located in /data_in/30000n-0.0015p-10h-0.02hp.in
Serial:
```
iteration number: 0    Needed 314 ms to finish Serial.
iteration number: 1    Needed 326 ms to finish Serial.
iteration number: 2    Needed 308 ms to finish Serial.
iteration number: 3    Needed 310 ms to finish Serial.
iteration number: 4    Needed 311 ms to finish Serial.
Average time needed for Serial compute: 313ms. Number of colours: 120
```

Parallel:
```
iteration number: 0    Needed 86 ms to finish Parallel.
iteration number: 1    Needed 84 ms to finish Parallel.
iteration number: 2    Needed 86 ms to finish Parallel.
iteration number: 3    Needed 84 ms to finish Parallel.
iteration number: 4    Needed 83 ms to finish Parallel.
Average time needed for Parallel compute: 84ms. Number of colours: 120
```

### Other recorded results: 
Executed on Ryzen R3 PRO 5450U, in WSL2, OS: Ubuntu 20.04.5 LTS on Windows 10 x86_64
For input 10000n-0.995p.in
```
iteration number: 0    Needed 4933 ms to finish parallel.
iteration number: 1    Needed 5597 ms to finish parallel.
iteration number: 2    Needed 5742 ms to finish parallel.
iteration number: 3    Needed 4789 ms to finish parallel.
iteration number: 4    Needed 5104 ms to finish parallel.

iteration number: 0    Needed 13756 ms to finish serial.
iteration number: 1    Needed 13227 ms to finish serial.
iteration number: 2    Needed 16870 ms to finish serial.
iteration number: 3    Needed 16155 ms to finish serial.
iteration number: 4    Needed 12972 ms to finish serial.
```
For input 30000n-0.0015p-10h-0.02hp.in
```
iteration number: 0    Needed 106 ms to finish parallel.
iteration number: 1    Needed 89 ms to finish parallel.
iteration number: 2    Needed 95 ms to finish parallel.
iteration number: 3    Needed 74 ms to finish parallel.
iteration number: 4    Needed 71 ms to finish parallel.

iteration number: 0    Needed 451 ms to finish serial.
iteration number: 1    Needed 423 ms to finish serial.
iteration number: 2    Needed 417 ms to finish serial.
iteration number: 3    Needed 421 ms to finish serial.
iteration number: 4    Needed 402 ms to finish serial.
```
For input 50000n-0.06p.in
```
iteration number: 0    Needed 15408 ms to finish parallel.
iteration number: 1    Needed 14221 ms to finish parallel.
iteration number: 2    Needed 14405 ms to finish parallel.
iteration number: 3    Needed 15120 ms to finish parallel.
iteration number: 4    Needed 19956 ms to finish parallel.

iteration number: 0    Needed 67376 ms to finish serial.
iteration number: 1    Needed 63061 ms to finish serial.
iteration number: 2    Needed 61760 ms to finish serial.
iteration number: 3    Needed 61515 ms to finish serial.
iteration number: 4    Needed 62632 ms to finish serial.
```
