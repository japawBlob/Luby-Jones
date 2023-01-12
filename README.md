# Luby Jones Algorytm

This repository is result of my work on Luby Jones algorythm, and it's parallelization.

This repository also contains graph generator written in python, that can be used to generate your own graphs.

### Recorded results: 
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

Jak vyřešit MPI?
Posílat nakonec každé iterace které nody se obarvily, aby si jemmohli obarvit všichni.