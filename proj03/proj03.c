//
// cs4900
// Project 03
// Sean McMeans
// w1566sxm
// Due 12 Feb 2026, accepted up to Feb 13 10pm
// System = fry
// Compiler syntax = ./compile.sh proj03
// Job Control File = proj03.sbatch
// Additional File  = NA
// Results file     = proj03.txt
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//#define DEBUG

// DO NOT CHANGE THIS FUNCTION!!!!!
// function to return a random variable between [0,1)
double randVar(){

  // Notes:
  // 2147483647 is the largest possible value of rand()
  // printf("max=%llu\n",RAND_MAX);

  return (double)((rand()%2000000000)/2000000000.0);
}


/* function to return M and P*/
unsigned long* calMandP(int N ) {

  //Save the results in an integer array
  static unsigned long MnP[2];

  // Constant for small square 
  // Reciprocal of sqrt(2) or 1/sqrt(2)
  double rsq2=0.70710678118654752440;

  // initialize counters each time the function is called
  unsigned long M=0, P=0;
  
  // random variables
  double x, y, rs;
  int randomPnts;

   // Pick N random (x,y) points
  for(randomPnts = 0; randomPnts < N; ++randomPnts) {

    x=randVar();
    y=randVar();

    // radius squared
    rs=x*x+y*y;

#ifdef DEBUG
    // test random values in function
    printf("%d x=%f y=%f rs=%f\n",randomPnts,x,y,rs);
#endif 

    if (rs<1){                   // or should it be rs<=1
      // Inside circle
      ++M;
      if (x<rsq2 && y<rsq2){     // or should it be <=
	// inside inner square
	++P;
      }
    }
  }

  MnP[0]=M;  
  MnP[1]=P;  
  return MnP;
}


int main (int argc, char *argv[]){

  // Initialize MPI
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Use current time as seed for random generator 
  srand(time(0) * rank); // Multiply by rank to ensure different seeds for each process
  
  // For convenience I passed "N" on the command line
  // Not checking for valid input
  char *a = argv[1];
  int N = atoi(a);
  
  // The Max value of unsigned long is 18,446,744,073,709,551,615
  unsigned long* MnP;   // Store in vector M=MnP[0] and P=MnP[1]
  unsigned long runTotalMnP[2]; // M and P total for each run
  unsigned long totalMnP[2];
  short converged = 0;
  double prev_pi_estimate = 0.0;
  long current_n = 0;
  while (converged == 0) {
    MnP=calMandP(N);

    // Sum M and P across all processes
    MPI_Reduce(MnP, runTotalMnP, 2, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Check for convergence
    if (rank == 0) {
      // Update current_n
      current_n += N;

      // Update total M and P
      totalMnP[0] += runTotalMnP[0]; // Total M
      totalMnP[1] += runTotalMnP[1]; // Total P

      // Calculate pi estimates
      double pi_estimate_M = 4.0 * (totalMnP[0] /(double)(current_n * world_size)); // 4 * (M / N)
      double pi_estimate_P = 2.0 * (totalMnP[0] / (double)totalMnP[1]); // 2 * (M / P)
      double pi_estimate = (pi_estimate_M + pi_estimate_P) / 2.0; // Average of the two estimates

      // Check if the estimate has converged
      if (prev_pi_estimate != 0.0 && fabs(pi_estimate - prev_pi_estimate) < 1e-12) {
        converged = 1;
      } else {
        prev_pi_estimate = pi_estimate;
      }
      
    }
    // Broadcast convergence status to all processes
    MPI_Bcast(&converged, 1, MPI_SHORT, 0, MPI_COMM_WORLD);

    // Print results after to make a graph of convergence
    if (rank == 0) {
      printf("M=%llu P=%llu\n",totalMnP[0],totalMnP[1]);
      printf("use(pi=4M/N) pi~%f\n",4.0*totalMnP[0]/(double)(current_n * world_size));
      printf("use(pi=2M/P) pi~%f\n",2.0*totalMnP[0]/totalMnP[1]);
    }
  }

  
  // Results
  // if (rank == 0) {
  //   printf("M=%llu P=%llu\n",totalMnP[0],totalMnP[1]);
  //   printf("use(pi=4M/N) pi~%f\n",4.0*totalMnP[0]/(N*world_size));
  //   printf("use(pi=2M/P) pi~%f\n",2.0*totalMnP[0]/totalMnP[1]);
  // }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  return 0;
}




