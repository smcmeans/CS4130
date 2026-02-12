#!/usr/bin/bash
#SBATCH -N 1
#SBATCH --tasks-per-node=4
#SBATCH --job-name=proj03
#SBATCH --output=proj03.txt
#SBATCH --time=00:30:00

## First make sure all other modules are not loaded
module purge

## Load the appropriate module
module load openmpi/4.0.3-gcc-8.5.0


## compile your code
mpicc proj03.c -o proj03.mpi

# Fix your environment
export OMPI_MCA_pml="^ucx"
export OMPI_MCA_btl_openib_if_include="mlx4_0:1"

# Make code executable
chmod u+x proj03.mpi

# Run on a a specified number of cores
mpirun -np 4 ./proj03.mpi 100000

