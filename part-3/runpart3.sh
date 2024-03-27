#!/bin/bash

# Compile part 3
mpicxx -fopenmp part3.cpp

# Set number of OpenMP Threads
# Run part 3 and specify number of MPI Ranks
OMP_NUM_THREADS=10 mpiexec -n 10 ./a.out 10000