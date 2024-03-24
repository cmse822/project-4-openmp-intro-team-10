#!/bin/bash

# Compile part1.cpp without OpenMP
g++ -o part1_no_omp.out part1_no_omp.cpp
./part1_no_omp.out > results_no_omp.txt

# Compile part1.cpp with OpenMP
g++ -o part1.out part1.cpp -fopenmp

# Run part1.out with different number of threads
./part1.out 1 > results_1_thread.txt
./part1.out 2 > results_2_thread.txt
./part1.out 4 > results_4_thread.txt
./part1.out 8 > results_8_thread.txt
./part1.out 16 > results_16_thread.txt
