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
	double local = 0.0, dx = (double)(b - a) / N; /* 小矩形宽度 */
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
		/* *0 0 号主节点，主要负责数据分发和结果收集* */
			for (int i = 0; i < N; ++i) /* 数据分发 : 0, * */
				MPI_Send(&i, 1, MPI_DOUBLE, i % numprocs + 1, 1, MPI_COMM_WORLD);
		for (int source = 1; source <= numprocs; ++source) /* 结果收集* */
		{ MPI_Recv(&local, 1, MPI_DOUBLE, source, 99, MPI_COMM_WORLD, &status); inte += local; }
		printf("The integal of x*x*x in region [%d,%d] =%16.15f\n", a, b, inte);
	}
	else
	{
		for (int i = myid - 1; i < N; i = i + numprocs)  /* * 各子节点接受数据计算开平方，本地累加* */
		{ MPI_Recv(&d, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status); 
		x = a + i * dx + dx / 2; /* 以每个矩形的中心点x值计算矩形高度 */
		local += x * x * x * dx; /* 矩形面积 = 高度x宽度=y*dx */ }
		MPI_Send(&local, 1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD); /*本地累加结果送回主节点*/

		printf("The integal of x*x*x of process %d on %s =%16.15f\n", myid-1, hostname, local);
	}
	end = MPI_Wtime();
	MPI_Reduce(&end, &maxe, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Finalize();

	if (myid == 0)
		printf("Runtime = %f\n", maxe-mins);
	return 0;
}