//
// cs4900
// Project 03
// Sean McMeans
// w1566sxm
// Due 08 Feb 2024, accepted up to Feb 09 10pm
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

  // Use current time as seed for random generator 
  srand(time(0) + rank); // Add rank to seed to ensure different seeds for each process
  
  // For convenience I passed "N" on the command line
  // Not checking for valid input
  char *a = argv[1];
  int N = atoi(a);
  
  // The Max value of unsigned long is 18,446,744,073,709,551,615
  unsigned long* MnP;   // Store in vector M=MnP[0] and P=MnP[1]
  unsigned long totalM = 0;
  unsigned long totalP = 0;
  short converged = 0;
  double prev_pi_estimate = 0.0;
  while (converged == 0) {
    MnP=calMandP(N);
    // Communicate M and P to rank 0 and calculate totals
    if (rank == 0) {
      totalM = MnP[0];
      totalP = MnP[1];
      int size;
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      for (int i = 1; i < size; ++i) {
        unsigned long recvMnP[2];
        MPI_Recv(&recvMnP[0], 2, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        totalM += recvMnP[0];
        totalP += recvMnP[1];
      }
      // Check for convergence by comparing the current estimate of pi with the previous estimate
      double pi_estimate = 4.0 * totalM / N;
      if (prev_pi_estimate != 0.0 && fabs(pi_estimate - prev_pi_estimate) < 1e-3) {
        converged = 1;
        
        
      } else {
        prev_pi_estimate = pi_estimate;
      }
      // Broadcast convergence status to all processes
      MPI_Bcast(&converged, 1, MPI_SHORT, 0, MPI_COMM_WORLD);

    } else {
      MPI_Send(&MnP, 2, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
      // Receive convergence status from rank 0
      MPI_Bcast(&converged, 1, MPI_SHORT, 0, MPI_COMM_WORLD);
    }
  }

  
  // Results
  if (rank == 0) {
    printf("M=%llu P=%llu\n",totalM,totalP);
    printf("use(pi=4M/N) pi~%f\n",4.0*totalM/N);
    printf("use(pi=2M/P) pi~%f\n",2.0*totalM/totalP);
  }
  
  return 0;
}




