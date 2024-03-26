#include "mpi.h"
#include <iostream>
#include <omp.h>
using namespace std;

int main() 
{
    // initilaized MPI
    MPI_Init(NULL, NULL);
    // Get rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Get size
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int _;
    MPI_Get_processor_name(processor_name, &_);
    // Print off a hello world message

    #pragma omp parallel
    {
        printf("Node: %s, Node %i out of %i, thread %i\n", processor_name, rank, size, omp_get_thread_num());
        if (rank==0){
            printf("Rank 0 node of %i nodes\n", size);
        }
    }
    MPI_Finalize();
    return 0;
}