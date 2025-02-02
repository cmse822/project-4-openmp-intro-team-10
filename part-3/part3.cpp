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
void MatMul (const Matrix& a, const Matrix& b, Matrix& c)
{
	for (int i = 0; i < a.dim * a.dim; ++i) {
        c.data[i] = 0;
    }
	
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

void PrintRow(double row[], int n)
{
	for (int i = 0; i < n; ++i)
	{
		std::cout << row[i] << " ";
	}
	std::cout << std::endl;
}

// Function to test matrices of size n
// It will perform "tests" number of tests to account for any variance, returning the average
double TestSize(int n, int rank, int process_count)
{	
	double timeSum = 0.0;
	// Sum of times for all the tests for this size
	if (rank == 0)
	{
		
		// Generate matrices, populate randomly for a and b (c starts as all 0s)
		Matrix a(n);
		Matrix b(n);
		Matrix c(n);
		a.GenerateElements(true);
		b.GenerateElements(true);

		Matrix sanity_check(n);
		MatMul(a, b, sanity_check);

		MPI_Status status;
		int processed_rows = 0;
		int row_index = 0;


		int availability[n];

		double start_time = omp_get_wtime();
		
		for (int i=1; i < process_count; i++){
			availability[i] = -1;
			double array[n*n];
			b.ToArray(array);
			// printf("MASTER SENDING Matrix B to %i \n",i);
			MPI_Send(&array,n*n,MPI_DOUBLE,i,0,MPI_COMM_WORLD);			
		}
		
		double row[n];
		double response_row[n];

		while (processed_rows < n)
		{
			for(int i=1; i < process_count; i++)
			{
				if(availability[i] == -1 & row_index < n){
					a.getRow(row, row_index);
					// printf("MASTER SENDING row %i to %i \n", processed_rows, i);
					MPI_Send(&processed_rows,1,MPI_INT,i,3,MPI_COMM_WORLD);
					MPI_Send(&row,n,MPI_DOUBLE,i,1,MPI_COMM_WORLD);			
					availability[i] = row_index;
					row_index ++;	
				}
			}

			for(int i=1; i < process_count; i++)
			{
				if(availability[i] != -1 & processed_rows <= n){
					// printf("MASTER RECEIVING RES row %i from %i \n", processed_rows, i);
					MPI_Recv(&response_row,n,MPI_DOUBLE, i,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
					// replace row availability[i] of c with response_row
					for (int j = 0; j < n; ++j)
					{
						c.data[availability[i] * n + j] = response_row[j];
					}
					
					availability[i] = -1;
					processed_rows += 1;
					// printf("### MASTER processed_rows %i\n", processed_rows);
					// printf("MASTER RECEIVED RES row %i from %i \n", processed_rows, i);
					MPI_Send(&processed_rows,1,MPI_INT,i,3,MPI_COMM_WORLD);
				}
			}

			
		if (processed_rows == n)
		{
			// printf("MASTER FINISHED\n");
			for(int i=1; i < process_count; i++)
			{
				MPI_Send(&processed_rows,1,MPI_INT,i,3,MPI_COMM_WORLD);
			}
		}	

		}


		double end_time = omp_get_wtime();	
		// check if c and sanity_check are equal
		// for (int i = 0; i < n; ++i)
		// {
		// 	for (int j = 0; j < n; ++j)
		// 	{
		// 		if (c.data[i * n + j] != sanity_check.data[i * n + j])
		// 		{
		// 			printf("ERROR: c[%i][%i] = %f, sanity_check[%i][%i] = %f\n", i, j, c.data[i * n + j], i, j, sanity_check.data[i * n + j]);
		// 		}
		// 	}
		// }
		
		timeSum += end_time - start_time;
	} else {
		// printf("WORKER %i STARTED \n",rank);
		double array[n*n] = {};
		double row[n] = {};
		
		// Matrix a(n);
		int processed_rows = 0;
		MPI_Recv(&array,n*n,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		// printf("WORKER %i RECEIVED MATRIX B\n",rank);

		while (processed_rows < n)
		{
			double res_row[n] = {};
			// printf("WORKER %i RECEIVING ROW TO MUL....\n",rank);
			MPI_Recv(&processed_rows,1,MPI_INT,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			if (processed_rows == n)
			{
				break;
			}
			MPI_Recv(&row,n,MPI_DOUBLE,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// printf("WORKER RECEIVED ROW \t");

			#pragma omp for collapse(2)
			for (int i = 0; i < n; ++i)
			{
				for (int j = 0; j < n; ++j)
				{
					res_row[i] +=  row[j] * array[j * n + i];
				}
			}
			// printf("WORKER %i SENDING BACK ROW....\n",rank);
			MPI_Send(&res_row,n,MPI_DOUBLE,0,2,MPI_COMM_WORLD);
			// printf("WORKER %i SENT ROW\n",rank);
			MPI_Recv(&processed_rows,1,MPI_INT,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// printf("WORKER %i RECEIVED processed_rows %i\n",rank, processed_rows);
		}

	}

	return timeSum;
}

// Function to output test data, should print to standard output
// The goal is to pipeline the data into python for plotting, ex: ./project_1 > python main.py
void OutputTestData(int test_size, double time_elapsed, int n_threads, int size_mpi)
{
    printf("%i \t %i \t %d \t %.8f\n", size_mpi, n_threads, test_size,  time_elapsed);
}

int main(int argc, char* argv[])
{
	// Keeping a simple list of sizes to check for now, a list for storing results, and how many tests for each size
	 // initilaized MPI
    int provided;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE) {
        std::cout << "MPI does not provide needed thread support." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
	int rank;
	int size_mpi;
  	MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// printf("RANKING %i \n",rank);
	int process_count;
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	int test_size = atoi(argv[1]);
	double test_res = TestSize(test_size, rank, process_count);
	if(rank == 0)
	{
		OutputTestData(test_size, test_res, omp_get_max_threads(), size_mpi);
	}


	MPI_Finalize();
	return 0;
	
}
