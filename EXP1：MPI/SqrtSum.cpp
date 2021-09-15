#include <unistd.h>
#include <stdio.h> 
#include <mpi.h> 
#include <math.h> 
#include <stdlib.h>
int main(int argc, char** argv)
{
	char hostname[100];
	int N= atoi(argv[1]);
	int myid, P, source, C = 0; 
	double SqrtSum = 0.0, d;
	double *data = new double[N];
	double start, end;
	double mins,maxe;
	int numprocs;
	for (int i = 0; i < N; ++i)
		data[i] = i * (i + 1) ;
	MPI_Status status; 
	MPI_Init(&argc, &argv);
	start = MPI_Wtime();
	MPI_Reduce(&start, &mins, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	gethostname(hostname,sizeof(hostname));
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 
	for (int i = myid; i < N; i = i + numprocs)
		SqrtSum += sqrt(data[i]);
	printf("The SqrtSum of process %d from host %s is %f\n", myid,hostname, SqrtSum);
	MPI_Reduce(&SqrtSum, &d, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (myid == 0) 
	{ 
		printf("The SqrtSum =%f\n", d);
	}
	
	end = MPI_Wtime();
	MPI_Reduce(&end, &maxe, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Finalize();

	if(myid==0)
		printf("Runtime = %f\n", maxe-mins);
	return 0;
}
