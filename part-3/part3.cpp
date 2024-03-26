#include <iostream>
#include <vector>
#include <random>
#include <omp.h>
#include "get_walltime.c"
#include "mpi.h"

// Matrix struct (no need to deal with private data, so class uneeded)
struct Matrix
{
	// Matrix has a dimension and a 1D array (simulates 2D)
	int dim;
	double* data;

	// Default constructor, matrix size 0
	Matrix()
	{
		dim = 0;
		data = nullptr;
	}

	// Constructor for given dimension, creates 0 matrix
	Matrix(int n)
	{
		dim = n;
		data = new double[n * n];
		for (int i = 0; i < dim * dim; ++i)
		{
			data[i] = 0;
		}
	}

	// Destructor
	~Matrix()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
	}

	// Resize the dimension of a matrix, costly since a new array is made
	void Resize(int n)
	{
		if (n != dim)
		{
			dim = n;
			delete[] data;
			data = new double[n * n];
		}
	}

	void ToArray(double arr[])
	{
		for (int i = 0; i < dim * dim; ++i)
		{
			arr[i] = data[i];
		}
	}

	void getRow(double arr[], int row)
	{
		for (int i = 0; i < dim; ++i)
		{
			arr[i] = data[row * dim + i];
		}
	}
	// Generates either a matrix of random integers (pass true) or 0 - n^2 in order if false/default
	void GenerateElements(bool random = false)
	{
		std::uniform_real_distribution<double> uniform(-10, 10);
		std::default_random_engine dre(std::random_device{}());
		for (int i = 0; i < dim * dim; ++i)
		{
			if (random)
			{
				data[i] = uniform(dre);
				continue;
			}
			data[i] = i;
		}
	}

	// Prints a matrix, very simple 2D format seperated by spaces and lines
	void DisplayMatrix()
	{
		for (int i = 0; i < dim; ++i)
		{
			for (int j = 0; j < dim; ++j)
			{
				std::cout << data[i * dim + j] << " ";
			}
			std::cout << std::endl;
		}
	}

	// Function to simulate 2D access, example: mat(x, y) simulates mat[x][y]
	double operator() (int x, int y)
	{
		return data[x * dim + y];
	}
};

// Matrix multiplication, takes in Matrix a, b and c, then performs c = a*b.
void MatMulWorker (const Matrix& a, const Matrix& b, Matrix& c, int rank, int process_count)
{
    #pragma omp for collapse(3)
	  for (int i = 0; i < a.dim; ++i)
	  {
		  for (int k = 0; k < a.dim; ++k)
		  {
			  for (int j = 0; j < a.dim; ++j)
			  {
				  c.data[i * a.dim + j] += a.data[i * a.dim + k] * b.data[k * a.dim + j];
			  }
		  }
	  }
}

// Function to test matrices of size n
// It will perform "tests" number of tests to account for any variance, returning the average
double TestSize(int n, int rank, int process_count)
{
	// Sum of times for all the tests for this size
	if (rank == 0)
	{
		double timeSum = 0.0;
    	// Generate matrices, populate randomly for a and b (c starts as all 0s)
		Matrix a(n);
		Matrix b(n);
		Matrix c(n);
		a.GenerateElements(true);
		b.GenerateElements(true);

		MPI_Status status;
		int processed_rows = 0;
		int availability[n];

    	double start_time = omp_get_wtime();
		
		for (int i=1; i < process_count; i++){
			availability[i] = 1;
			double array[n*n];
			a.ToArray(array);
			printf("SENDING to %i \n",i);
			MPI_Send(&array,n*n,MPI_DOUBLE,i,0,MPI_COMM_WORLD);			
		}
		
		double row[n*n];
		double response_row[n*n];

		while (processed_rows < n)
		{

			
			
			
			for(int i=1; i < process_count; i++)
			{
				if(availability[i] == 1){
					b.getRow(row,processed_rows);
					printf("SENDING row %i to %i \n", processed_rows, i);
					MPI_Send(&row,n*n,MPI_DOUBLE,i,0,MPI_COMM_WORLD);			
					availability[i] = 0;
					
				}
			}

			// for(int i=1; i < process_count; i++)
			// {
			// 	if(availability[i] == 0){
			// 		printf("RECEIVING RES row %i from %i \n", processed_rows, i);
			// 		MPI_Recv(&response_row,n,MPI_INT, i,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// 		availability[i] = 1;
			// 		// MPI_Send(&processed_rows,n*n,MPI_INT,i,0,MPI_COMM_WORLD);
			// 		processed_rows += 1;
			// 	}
			// }
			

			 
			// }
			// {
			// 	MPI_Recv(&Vector[0],n,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			// 	processed_rows++;
			// 	double array[n*n];
			// 	a.ToArray(array);
			// 	MPI_Send(&array,n*n,MPI_DOUBLE,status.MPI_SOURCE,0,MPI_COMM_WORLD);
			// }
			// int res;
		    // MPI_Recv(&res, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			// process_count--;
			// MPI_Send(&process_count,n*n,MPI_INT,status.MPI_SOURCE,0,MPI_COMM_WORLD);
		}
		
		
    	double end_time = omp_get_wtime();
    
    	// Final Timing
		timeSum += end_time - start_time;
		
		return timeSum;
	} else {
		printf("WORKER STARTED......");
		double array[n*n];
		double row[n*n];

		// Matrix a(n);
		int processed_rows = 0;
		MPI_Recv(&array,n*n,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		printf("WORKER RECEIVED......");

		while (processed_rows < n)
		{
			printf("WORKER SENDING......");
			MPI_Send(&row,n*n,MPI_DOUBLE,0,1,MPI_COMM_WORLD);

			// MPI_Recv(&processed_rows,1,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
		
		// MatMul(a, b, c, rank, process_count);

	}
	return 0;
}

// Function to output test data, should print to standard output
// The goal is to pipeline the data into python for plotting, ex: ./project_1 > python main.py
void OutputTestData(int test_size, double time_elapsed, int n_threads)
{
    printf("%d \t %d \t %.8f\n", n_threads, test_size,  time_elapsed);
}

int main(int argc, char* argv[])
{
	// Keeping a simple list of sizes to check for now, a list for storing results, and how many tests for each size
	 // initilaized MPI
	MPI_Init(NULL, NULL);
	int rank;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("RANKING %i",rank);
	int process_count;
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	if (rank == 0)
	{
		printf("Total threads: %i\n",omp_get_max_threads());
	}
	
	std::vector<int> test_sizes = {20, 100, 1000};
	std::vector<double> test_results(test_sizes.size(), 0);
	// Run tests on each size
	for (int i = 0; i < test_sizes.size(); ++i)
	{	
		printf("DEBUG");
		test_results[i] = TestSize(test_sizes[i], rank, process_count);
		if (rank == 0)
		{
			OutputTestData(test_sizes[i], test_results[i], omp_get_thread_num());
		}
	}


	MPI_Finalize();
	return 0;
	
}
