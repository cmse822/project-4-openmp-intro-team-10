#include <iostream>
#include <vector>
#include <random>
#include <omp.h>
#include "get_walltime.c"
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
  #pragma omp parallel
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
}

// Function to test matrices of size n
// It will perform "tests" number of tests to account for any variance, returning the average
double TestSize(int n, int tests)
{
	// Sum of times for all the tests for this size
	double timeSum = 0.0;
    // Run a certain number of tests
	for (int t = 0; t < tests; ++t)
	{

    // Generate matrices, populate randomly for a and b (c starts as all 0s)
		Matrix a(n);
		Matrix b(n);
		Matrix c(n);
		a.GenerateElements(true);
		b.GenerateElements(true);

    // Matrix Multiplication
    double start_time = omp_get_wtime();
		MatMul(a, b, c);
    double end_time = omp_get_wtime();
    
    // Final Timing
		timeSum += end_time - start_time;
  }

	// Return the average time taken
	return timeSum / tests;
}

// Function to output test data, should print to standard output
// The goal is to pipeline the data into python for plotting, ex: ./project_1 > python main.py
void OutputTestData(int test_size, double time_elapsed)
{
    printf("%d \t %.8f\n", test_size,  time_elapsed);
}

int main(int argc, char* argv[])
{
  int n_threads = 1;
  if (argc == 2)
    n_threads = atoi(argv[1]);
  omp_set_num_threads(n_threads);
	
  // Keeping a simple list of sizes to check for now, a list for storing results, and how many tests for each size
	std::vector<int> test_sizes = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
	std::vector<double> test_results(test_sizes.size(), 0);
	int test_quantity = 100;

	// Run tests on each size
	for (int i = 0; i < test_sizes.size(); ++i)
	{
		test_results[i] = TestSize(test_sizes[i], test_quantity);
    OutputTestData(test_sizes[i], test_results[i]);
	}
}
