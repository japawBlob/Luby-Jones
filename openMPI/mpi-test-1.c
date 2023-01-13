#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static inline double f(double x)
{
  return 2*x+1;
}

int main(int argc, char *argv[])
{
  int myid, numprocs;
  unsigned long int i, n = 0;
  double startwtime, endwtime, a = 4.0, b = 5.0;
  double total, integral = 0.0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); // obtain number
                                            // of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);     // obtain process number

  //int arr [] = {1, 1, 2, 4, 5, 1, 2, 5, 1, 1, 1, 1, 1, 4, 5, 1, 4, 5, 1, 2, 5, 1, 1, 1, 1, 1, 4, 5, 1, 2, 5, 1, 1, 1, 1, 1, 4, 5, 1, 4, 5, 1, 2, 5, 1, 1, 1, 1, 1, 9};
  int arr [] = {1, 1, 2, 4, 1, 9, 1, 2, 1, 1};

  size_t arr_size = sizeof(arr)/sizeof(int);
  int chunk_size = (arr_size+numprocs-1)/numprocs;
  if (myid == 0) printf("arrsize: %d, chunk_size: %d, remainder: %d\n", arr_size, chunk_size, arr_size%numprocs);

  int * coloured_nodes = (int*)malloc(chunk_size*sizeof(int));
  memset(coloured_nodes, 0, chunk_size*sizeof(int));
  int number_of_coloured_nodes = 0;
  int number_of_iterations = 0;
  for (int i = myid*(chunk_size); i < MIN(myid*chunk_size+chunk_size, arr_size); i++){
    // printf("I am %d, I am on %d iteration and my number is %d\n", myid, i, arr[i]);
    if(arr[i] < 3){
      coloured_nodes[number_of_coloured_nodes++] = i;
    }
    number_of_iterations++;
  }
  for (int i = 0; i < chunk_size; i++){
    printf("%d ", coloured_nodes[i]);
  }
  printf("\n");
  int * complete_data = (int*)malloc(arr_size*sizeof(int)+chunk_size*sizeof(int));
  int * counts_per_process = (int*)malloc(numprocs*sizeof(int)+sizeof(int));
  MPI_Allgather(&number_of_coloured_nodes, 1, MPI_INT, counts_per_process, 1, MPI_INT, MPI_COMM_WORLD);
  printf("I am %d, I finished my work in %d iterations\n", myid, number_of_iterations);
  for (int i = 0; i < numprocs; i++){
    printf("%d ", counts_per_process[i]);
  }
  printf("\n");

  MPI_Allgather(coloured_nodes, chunk_size, MPI_INT, complete_data, chunk_size, MPI_INT, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  for (int i = 0; i < numprocs; i++){
    printf("%d ", counts_per_process[i]);
  } printf("\n");
  if (myid == 0){

    for (int i = 0; i<numprocs; i+=1){
      printf("Data from process: %d are    ",i);
      for (int k = 0; k < counts_per_process[i]; k++){
        printf("%d ",complete_data[i*chunk_size+k]);
      }
    }
  }
  
  

  // do {
  //   if (myid == 0) {
  //     printf("\n Enter number of intervals (0 to exit): "); fflush(stdout);
  //     scanf("%lu", &n);
  //     startwtime = MPI_Wtime();
  //   }

  //   // send variable n, with vector length 1, type int, from process #0 to others
  //   MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  //   if(n==0) break;

  //   double h = (b - a) / n;
  //   double i1 = myid*(n/ numprocs);
  //   double i2 = (myid+1)*(n/numprocs);

  //   integral= ( f(a+i1*h) + f(a+i2*h) ) / 2;

  //   for( i=i1+1 ; i<i2 ; i++ )
  //     integral += f(a+i*h);

  //   // summarize integral variable from all processes to total in process 0,
  //   // length 1, type double, operation sum/addition
  //   MPI_Reduce(&integral, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  //   if (myid == 0) {
  //     endwtime = MPI_Wtime();
  //     printf("I= %f\n", total);
  //       printf("spent time: %f s\n", endwtime - startwtime); fflush(stdout);
  //   }
  // } while (1);

  MPI_Finalize();
  return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// void printdata(int size, int rank, int n, int *data) {
//     printf("Rank %d\n",rank);
//     for (int j=0; j<size*n; j++)
//         printf("%d ",data[j]);
//     printf("\n");
// }

// int main(int argc, char **argv) {
//     const int n=3;
//     int ierr, rank, size;
//     int *datain, *dataout;

//     ierr = MPI_Init(&argc, &argv);
//     ierr|= MPI_Comm_size(MPI_COMM_WORLD,&size);
//     ierr|= MPI_Comm_rank(MPI_COMM_WORLD,&rank);

//     datain = (int *)malloc(n*size*sizeof(int));
//     dataout = (int *)malloc(n*size*sizeof(int));
//     for (int i=0; i<n*size; i++)
//         datain[i]=9;
//     for (int i=0; i<n; i++)
//         datain[rank*n+i]=rank;

//     if (rank == 0) printf("Before:\n");
//     printdata(size, rank, n, datain);

//     MPI_Allgather(&(datain[rank*n]), n, MPI_INT, dataout, n, MPI_INT, MPI_COMM_WORLD);

//     if (rank == 0) printf("After:\n");
//     printdata(size, rank, n, dataout);

//     free(datain);
//     free(dataout);
//     MPI_Finalize();
//     return 0;
// }