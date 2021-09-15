#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
int main(int argc, char** argv)
{
	int N= atoi(argv[1]), a = 10, b = 100;
	char hostname[100];
	int myid, numprocs;
	int d;
	double local = 0.0, dx = (double)(b - a) / N; /* С���ο�� */
	double inte=0.0, x,start,end,mins,maxe;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	gethostname(hostname, sizeof(hostname));
	start = MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	numprocs--;
	MPI_Reduce(&start, &mins, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	if (myid == 0) {
		/* *0 0 �����ڵ㣬��Ҫ�������ݷַ��ͽ���ռ�* */
			for (int i = 0; i < N; ++i) /* ���ݷַ� : 0, * */
				MPI_Send(&i, 1, MPI_DOUBLE, i % numprocs + 1, 1, MPI_COMM_WORLD);
		for (int source = 1; source <= numprocs; ++source) /* ����ռ�* */
		{ MPI_Recv(&local, 1, MPI_DOUBLE, source, 99, MPI_COMM_WORLD, &status); inte += local; }
		printf("The integal of x*x*x in region [%d,%d] =%16.15f\n", a, b, inte);
	}
	else
	{
		for (int i = myid - 1; i < N; i = i + numprocs)  /* * ���ӽڵ�������ݼ��㿪ƽ���������ۼ�* */
		{ MPI_Recv(&d, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status); 
		x = a + i * dx + dx / 2; /* ��ÿ�����ε����ĵ�xֵ������θ߶� */
		local += x * x * x * dx; /* ������� = �߶�x���=y*dx */ }
		MPI_Send(&local, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD); /*�����ۼӽ���ͻ����ڵ�*/

		printf("The integal of x*x*x of process %d on %s =%16.15f\n", myid-1, hostname, local);
	}
	end = MPI_Wtime();
	MPI_Reduce(&end, &maxe, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Finalize();

	if (myid == 0)
		printf("Runtime = %f\n", maxe-mins);
	return 0;
}