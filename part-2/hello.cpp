#include "mpi.h"
#include <iostream>
#include <omp.h>
using namespace std;

int main() {
    cout << "Before MPI" << endl;

    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE) {
        cout << "MPI does not provide needed thread support." << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    #pragma omp parallel
    {
        printf("Node: %d out of %d, thread %d\n", rank, size, omp_get_thread_num());
        if (rank == 0) {
            printf("Rank 0 of : %d nodes\n", size);
        }
    }

    MPI_Finalize();
    cout << "After MPI" << endl;
    return 0;
}
