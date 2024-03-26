## Part 1

## Part 2

This is the output of the hello world model with different number of OMP threads and MPI processes. 
```
------------------------------------------------------------------------
(base) -bash-4.2$ OMP_NUM_THREADS=1 mpiexec -n 2 a.out
Node: dev-intel16, Node 0 out of 2, thread 0
Rank 0 node of 2 nodes
Node: dev-intel16, Node 1 out of 2, thread 0
------------------------------------------------------------------------
(base) -bash-4.2$ OMP_NUM_THREADS=2 mpiexec -n 2 a.out
Node: dev-intel16, Node 0 out of 2, thread 0
Rank 0 node of 2 nodes
Node: dev-intel16, Node 0 out of 2, thread 1
Rank 0 node of 2 nodes
Node: dev-intel16, Node 1 out of 2, thread 0
Node: dev-intel16, Node 1 out of 2, thread 1
------------------------------------------------------------------------
(base) -bash-4.2$ OMP_NUM_THREADS=2 mpiexec -n 4 a.out
Node: dev-intel16, Node 0 out of 4, thread 0
Rank 0 node of 4 nodes
Node: dev-intel16, Node 2 out of 4, thread 0
Node: dev-intel16, Node 2 out of 4, thread 1
Node: dev-intel16, Node 3 out of 4, thread 0
Node: dev-intel16, Node 3 out of 4, thread 1
Node: dev-intel16, Node 0 out of 4, thread 1
Rank 0 node of 4 nodes
Node: dev-intel16, Node 1 out of 4, thread 0
Node: dev-intel16, Node 1 out of 4, thread 1
------------------------------------------------------------------------
(base) -bash-4.2$ OMP_NUM_THREADS=4 mpiexec -n 2 a.out
Node: dev-intel16, Node 0 out of 2, thread 0
Rank 0 node of 2 nodes
Node: dev-intel16, Node 0 out of 2, thread 1
Rank 0 node of 2 nodes
Node: dev-intel16, Node 0 out of 2, thread 2
Rank 0 node of 2 nodes
Node: dev-intel16, Node 0 out of 2, thread 3
Rank 0 node of 2 nodes
Node: dev-intel16, Node 1 out of 2, thread 0
Node: dev-intel16, Node 1 out of 2, thread 2
Node: dev-intel16, Node 1 out of 2, thread 1
Node: dev-intel16, Node 1 out of 2, thread 3

```
If we take a close look at the results, we can see that in case where we have 4 nodes and 2 threads or vice versa, we have statements which claim rank 0 exactly to the number of MPI processes which is exactly as we expetcted. Also, we can see that the threads don't have any specific order.


## Part 3
