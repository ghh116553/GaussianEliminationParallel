#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

int
main (int argc, char **argv)
{
	//random seed used to generate
	//the A and B matrices
	unsigned int seed = time(NULL);

	//grab 'n' from console
	int n = atoi(argv[1]);

	int row, col, i, j;

	//malloc the A, x, b and matrices for each scheduling paradigm
	int **A = (int **) malloc(sizeof(int*) * n);
	for (i = 0; i < n; ++i)
		A[i] = (int *) malloc(sizeof(int) * n);

	int *x_g = (int *) malloc(sizeof(int) * n);

	int *x_s = (int *) malloc(sizeof(int) * n);

	int *x_d = (int *) malloc(sizeof(int) * n);

	int *b = (int *) malloc(sizeof(int) * n);

	//this matrix is for the serial code only!
	int *x = (int *) malloc(sizeof(int) * n);

	//fill A and b matrix with random numbers
	for (i = 0; i < n; ++i) {
		for (j = 0; j < n; ++j)
			A[i][j] = (rand_r(&seed) % 10) + 1;
		b[i] = (rand_r(&seed) % 10);
	}

	printf("Created Matrices...\n\n");

	//begin timing serial code
	struct timeval stop, start;
	gettimeofday (&start, NULL);

	//serial row-solution
	for(row = n - 1; row >= 0; row--) {
		x[row] = b[row];

		for(col = row + 1; col < n; col++)
			x[row] -= A[row][col] * x[col];

		x[row] /= A[row][row];
	}

	//end timing serial code
	gettimeofday(&stop, NULL);

	//output serial timings
    long elapsed = (stop.tv_sec-start.tv_sec)*1000000 + stop.tv_usec-start.tv_usec;
    printf("Serial solution time: %ld ms\n\n", elapsed);

	//begin timing parallel (guided) code
	struct timeval stop_g, start_g;
	gettimeofday(&start_g, NULL);

	//parallel row-solution
	for(row = n - 1; row >= 0; row--) {
		x_g[row] = b[row];

		int redux = 0;
#		pragma omp parallel for reduction(+: redux) schedule(guided)
		for(col = row + 1; col < n; col++)
			redux += A[row][col] * x_g[col];

		x_g[row] -= redux;
		x_g[row] /= A[row][row];
	}

	//end timing parallel code
	gettimeofday(&stop_g, NULL);

	//output parallel timings
        elapsed = (stop_g.tv_sec-start_g.tv_sec)*1000000 + stop_g.tv_usec-start_g.tv_usec;
        printf("Parallel solution (guided) time: %ld ms\n\n", elapsed);

	//begin timing parallel (static) code
	struct timeval stop_s, start_s;
	gettimeofday(&start_s, NULL);

	//parallel row-solution
	for(row = n - 1; row >= 0; row--) {
		x_s[row] = b[row];

		int redux = 0;
#		pragma omp parallel for reduction(+: redux) schedule(static)
		for(col = row + 1; col < n; col++)
			redux += A[row][col] * x_s[col];

		x_s[row] -= redux;
		x_s[row] /= A[row][row];
	}

	//end timing parallel code
	gettimeofday(&stop_s, NULL);

	//output parallel timings
    elapsed = (stop_s.tv_sec-start_s.tv_sec)*1000000 + stop_s.tv_usec-start_s.tv_usec;
    printf("Parallel solution (static) time: %ld ms\n\n", elapsed);

	//begin timing parallel (dynamic) code
	struct timeval stop_d, start_d;
	gettimeofday(&start_d, NULL);

	//parallel row-solution
	for(row = n - 1; row >= 0; row--) {
		x_d[row] = b[row];

		int redux = 0;
#		pragma omp parallel for reduction(+: redux) schedule(dynamic)
		for(col = row + 1; col < n; col++)
			redux += A[row][col] * x_d[col];

		x_d[row] -= redux;
		x_d[row] /= A[row][row];
	}

	//end timing parallel code
	gettimeofday(&stop_d, NULL);

	//output parallel timings
    elapsed = (stop_d.tv_sec-start_d.tv_sec)*1000000 + stop_d.tv_usec-start_d.tv_usec;
    printf("Parallel solution (dynamic) time: %ld ms\n\n", elapsed);

	free(A);
	free(b);
	free(x);
	free(x_s);
	free(x_g);
	free(x_d);

	return 0;
}

