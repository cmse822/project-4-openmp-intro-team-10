## Part 1

## Part 2

Here is the output of the hello world model with 4 threads and 2 MPI processes. The code for this part is in `part-2/hello.cpp`.
```
Before MPI
Before MPI
Node: 0 out of 2, thread 0
Rank 0 of : 2 nodes
Node: 0 out of 2, thread 2
Rank 0 of : 2 nodes
Node: 0 out of 2, thread 1
Rank 0 of : 2 nodes
Node: 0 out of 2, thread 3
Rank 0 of : 2 nodes
Node: 1 out of 2, thread 0
Node: 1 out of 2, thread 3
Node: 1 out of 2, thread 1
Node: 1 out of 2, thread 2
After MPI
After MPI
```
First of all we can see that `Before MPI` and `After MPI` is printed twice showing the two mpi processes. As we can see there are 4 prints with rank 0. This shows that the print statement is ran with four threads in the rank 0 process. If we take a close look at the results, we can see that the threads don't have any specific order which is expected.

## Part 3


