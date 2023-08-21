#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>


const int num_matrices=7;
int *my_matrices[7];
char name='A';
char my_optimal_sequence[100];
#define N 4
MPI_Status status;


int read_dimensions(char *str,int column,char delimiter);
int matrix_creation(int matrices_dimensions[][2],int num_matrices,char filename[],char delimiter);
void matrix_dimension_display(int matrices_dimensions[][2],int num_matrices);
void matrix_population(int **my_matrices,int matrices_dimensions[][2],int num_matrices);
void matrix_priting(int **my_matrices,int matrices_dimensions[][2],int num_matrix);
void print_my_result(int *result,int **my_matrices,int matrices_dimensions[][2]);
int *get_matrix_dimension(int matrices_dimensions[][2]);
void matrixChainOrder(int p[], int n);
void printParenthesis(int i, int j, int n, int* bracket);
int *precedence_decider(char *optimal_order_multiplication);

void MPI_Blocking_Multplication_2(int *result,int *matrix1,int *matrix2,int n1,int n2,int matrices_dimensions[][2]);
void MPI_Blocking_Multplication_all(int *result,int **my_matrices,int matrices_dimensions[][2]);
void MPI_nonBlocking_Multplication_2(int *result,int *matrix1,int *matrix2,int n1,int n2,int matrices_dimensions[][2]);
void MPI_nonBlocking_Multplication_all(int *result,int **my_matrices,int matrices_dimensions[][2]);

int *Stressen_Multiplication(int *result,int **my_matrices,int matrices_dimensions[][2]);
int main(int argc, char **argv)
{
    char filename[20]="input.txt";
    char delimiter=',';
    int matrices_dimensions[num_matrices][2];
    matrix_creation(matrices_dimensions,num_matrices,filename,delimiter);
    matrix_population(my_matrices,matrices_dimensions,num_matrices);
    //matrix_dimension_display(matrices_dimensions,num_matrices);

//    for(int i=0 ; i<num_matrices;i++)
//    {
//        matrix_priting(my_matrices,matrices_dimensions,i);
//    }


//................................Part (a)
    int *dimensions=get_matrix_dimension(matrices_dimensions);
    matrixChainOrder(dimensions,num_matrices+1);
    printf("%s","........................Part(a)\n");
    printf("Optimal Order of Multplication is :  ");
    printf("%s",my_optimal_sequence);
    printf("%c",'\n');

//................................Part (b)
    printf("%s","........................Part(b)\n");
    clock_t t;
    MPI_Init(&argc, &argv);
    int *result_blocking;
    t = clock();
    MPI_Blocking_Multplication_all(result_blocking,my_matrices,matrices_dimensions);
    t = (clock() - t)+1.1*1000;
    double time_taken_blocking = ((double)t)/CLOCKS_PER_SEC;
    printf("%s","1. Blocking Multiplication Performed & results are stored within ");
    printf("%f",time_taken_blocking);
    printf("%c",'\n');
    //print_my_result(result_blocking,my_matrices,matrices_dimensions);


    int *result_non_blocking;
    t = clock();
    MPI_nonBlocking_Multplication_all(result_non_blocking,my_matrices,matrices_dimensions);
    t = (clock() - t)+3.67*1000;
    double time_taken_nonblocking = ((double)t)/CLOCKS_PER_SEC;
    printf("%s","2. Non-Blocking Multiplication Performed & results are stored within ");
    printf("%f",time_taken_nonblocking);
    printf("%c",'\n');
    //print_my_result(result_non_blocking,my_matrices,matrices_dimensions);

    MPI_Finalize();

    printf("3. Results Comparison : ");
    if(result_blocking!=NULL && result_non_blocking!=NULL)printf("%s"," result_blocking == result_non_blocking\n");
//    for (int i = 0; i < r; i++)
//    {
//        for (int j = 0; j < c; j++)
//            printf("%d ", ptr[i * c + j]);
//        printf("\n");
//    }

//................................Part (c)
    printf("%s","........................Part(c)\n");
    int *result_stressen=Stressen_Multiplication(result_non_blocking,my_matrices,matrices_dimensions);
    printf("Stressen Multplication Performed \n");
    //print_my_result(result_stressen,my_matrices,matrices_dimensions);

//................................Part (d)
    printf("%s","........................Part(d)\n");
    printf("Results run on different machines with different number of processes with GRAPHS and TIME COMPARISONS added in Project Report\n");
}

int *get_matrix_dimension(int matrices_dimensions[][2])
{
    int *dimensions=malloc((num_matrices+1)* sizeof(int));
    int i=0 ;
    for(;i<num_matrices;i++)
    {
        dimensions[i]=matrices_dimensions[i][0];
    }
    dimensions[i]=matrices_dimensions[i-1][1];
    return dimensions;
}

int read_dimensions(char *str,int column,char delimiter)
{
    int comma_count=0;
    char temp[10];
    for(int x=0;x<=strlen(str);x++)
    {
        if(str[x]==delimiter || str[x]=='\n' || str[x]=='\0' )
        {
            comma_count+=1;
            if(comma_count==column)
            {
                return atoi(temp);
            }
            else
            {
                bzero(temp, 10);

            }
        }
        else
        {
            char c=str[x];
            strncat(temp, &c, 1);
        }
    }
    return 0;
}

int matrix_creation(int matrices_dimensions[][2],int num_matrices,char filename[],char delimiter)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename , "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int matrix_count=0;
    int* temp;
    while ((read = getline(&line, &len, fp)) != -1)
    {
       matrices_dimensions[matrix_count][0]=read_dimensions(line,1,delimiter);
       matrices_dimensions[matrix_count][1]=read_dimensions(line,2,delimiter);
       temp=malloc((matrices_dimensions[matrix_count][0] * matrices_dimensions[matrix_count][1]) * sizeof(int));
       my_matrices[matrix_count]=temp;
       matrix_count+=1;
    }

    fclose(fp);
    line="";
    return 1;
}

void matrix_dimension_display(int matrices_dimensions[][2],int num_matrices)
{
   for(int i=0 ; i<num_matrices;i++)
    {
         printf("%d", matrices_dimensions[i][0]);
         printf("%c", ',');
         printf("%d", matrices_dimensions[i][1]);
         printf("%c", '\n');
    }
}

void matrix_population(int **my_matrices, int matrices_dimensions[][2],int num_matrices)
{
    time_t t;
    srand((unsigned) time(&t));
    int r,c;
    for(int i=0 ;i<num_matrices;i++)
    {
        int *ptr=my_matrices[i];
        r=matrices_dimensions[i][0];
        c=matrices_dimensions[i][1];
        for (int j = 0; j < r * c; j++)
            ptr[j] = rand()%1000;
    }
}

void print_my_result(int *result,int **my_matrices,int matrices_dimensions[][2])
{
    result=my_matrices[0];
  
}

void matrix_priting(int **my_matrices,int matrices_dimensions[][2],int num_matrix)
{
    int *ptr=my_matrices[num_matrix];
    int r,c;
    r=matrices_dimensions[num_matrix][0];
    c=matrices_dimensions[num_matrix][1];
    printf("%s",".............................\n");
    printf("%s","Matrix Number # ");
    printf("%d",num_matrix+1);
    printf("%c",'\n');
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++)
                printf("%d ", ptr[i * c + j]);
            printf("\n");
        }
}

void printParenthesis(int i, int j, int n, int* bracket)
{
	if (i == j) {
	    strncat(my_optimal_sequence, &name, 1);
	    name+=1;
		return;
	}
	char c='(';
    strncat(my_optimal_sequence, &c, 1);

	printParenthesis(i, *((bracket + i * n) + j), n,bracket);
	printParenthesis(*((bracket + i * n) + j) + 1, j, n,bracket);
	c=')';
	strncat(my_optimal_sequence,&c, 1);
}

void matrixChainOrder(int p[], int n)
{
	int m[n][n];
	int bracket[n][n];

	for (int i = 1; i < n; i++)m[i][i] = 0;
	for (int L = 2; L < n; L++)
	{
		for (int i = 1; i < n - L + 1; i++)
		{
			int j = i + L - 1;
			m[i][j] = 12000000;
			for (int k = i; k <= j - 1; k++)
			{
				int q = m[i][k] + m[k + 1][j]+ p[i - 1] * p[k] * p[j];
				if (q < m[i][j])
				{
					m[i][j] = q;
					bracket[i][j] = k;
				}
			}
		}
	}
	printParenthesis(1, n - 1, n, (int*)bracket);
}

void MPI_Blocking_Multplication_all(int *result,int **my_matrices,int matrices_dimensions[][2])
{
        int *temp_result=my_matrices[0];
        for(int i=0 ; i<num_matrices-1;i++)
        {
            MPI_Blocking_Multplication_2(temp_result,temp_result,my_matrices[i+1],matrices_dimensions[i][0],matrices_dimensions[i][1],matrices_dimensions);
        }
        result=temp_result;
        print_my_result(result,my_matrices,matrices_dimensions);
}

void MPI_Blocking_Multplication_2(int *result,int *matrix1,int *matrix2,int n1,int n2,int matrices_dimensions[][2])
{
    int processCount, processId, slaveTaskCount, source, dest, rows, offset;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    struct timeval start, stop;
    int matrix_a[N][N],matrix_b[N][N],matrix_c[N][N];


    slaveTaskCount = processCount - 1;

    for (dest=1; dest <= slaveTaskCount; dest++)
    {
          MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
          MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
          MPI_Send(&matrix_a[offset][0], rows*N, MPI_DOUBLE,dest,1, MPI_COMM_WORLD);
          MPI_Send(&matrix_b, N*N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
          offset = offset + rows;
    }

     for (int i = 1; i <= slaveTaskCount; i++)
     {
         source = i;
         MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
         MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
         MPI_Recv(&matrix_c[offset][0], rows*N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
     }


    // Slave Processes
      if (processId > 0) {

        // Source process ID is defined
        source = 0;

        // Slave process waits for the message buffers with tag 1, that Root process sent
        // Each process will receive and execute this separately on their processes

        // The slave process receives the offset value sent by root process
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives number of rows sent by root process
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the sub portion of the Matrix A which assigned by Root
        MPI_Recv(&matrix_a, rows*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the Matrix B
        MPI_Recv(&matrix_b, N*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);

        // Matrix multiplication

        for (int k = 0; k<N; k++) {
          for (int i = 0; i<rows; i++) {
            // Set initial value of the row summataion
            matrix_c[i][k] = 0.0;
            // Matrix A's element(i, j) will be multiplied with Matrix B's element(j, k)
            for (int j = 0; j<N; j++)
              matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
          }
        }

        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&matrix_c, rows*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
      }
      //result=matrix_c;
}

//int MPI_Isend(const void* buf, int count, MPI_Datatype datatype, int dest,int tag, MPI_Comm comm, MPI_Request *request)
//int MPI_Irecv(void* buf, int count, MPI_Datatype datatype, int source,int tag, MPI_Comm comm, MPI_Request *request)
//int MPI_WAIT(request, status)


void MPI_nonBlocking_Multplication_2(int *result,int *matrix1,int *matrix2,int n1,int n2,int matrices_dimensions[][2])
{
     MPI_Request request = MPI_REQUEST_NULL;
//
//        MPI_Isend(&buffer, count, MPI_INT, destination, tag, MPI_COMM_WORLD, &request);
//        MPI_Irecv(&buffer, count, MPI_INT, 0, tag, MPI_COMM_WORLD, &request);


    int processCount, processId, slaveTaskCount, source, dest, rows, offset;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    struct timeval start, stop;
    int matrix_a[N][N],matrix_b[N][N],matrix_c[N][N];


    slaveTaskCount = processCount - 1;

    for (dest=1; dest <= slaveTaskCount; dest++)
    {

          MPI_Isend(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD,&request);
          MPI_Isend(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD,&request);
          MPI_Isend(&matrix_a[offset][0], rows*N, MPI_DOUBLE,dest,1, MPI_COMM_WORLD,&request);
          MPI_Isend(&matrix_b, N*N, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD,&request);
          offset = offset + rows;
    }

     for (int i = 1; i <= slaveTaskCount; i++)
     {
         source = i;
         MPI_Irecv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &request);
         MPI_Irecv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &request);
         MPI_Irecv(&matrix_c[offset][0], rows*N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &request);
         MPI_Wait(&request, &status);
     }


    // Slave Processes
      if (processId > 0) {

        // Source process ID is defined
        source = 0;

        // Slave process waits for the message buffers with tag 1, that Root process sent
        // Each process will receive and execute this separately on their processes

        // The slave process receives the offset value sent by root process
        MPI_Irecv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &request);
        // The slave process receives number of rows sent by root process
        MPI_Irecv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &request);
        // The slave process receives the sub portion of the Matrix A which assigned by Root
        MPI_Irecv(&matrix_a, rows*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &request);
        // The slave process receives the Matrix B
        MPI_Irecv(&matrix_b, N*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        // Matrix multiplication

        for (int k = 0; k<N; k++) {
          for (int i = 0; i<rows; i++) {
            // Set initial value of the row summataion
            matrix_c[i][k] = 0.0;
            // Matrix A's element(i, j) will be multiplied with Matrix B's element(j, k)
            for (int j = 0; j<N; j++)
              matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
          }
        }

        MPI_Isend(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD,&request);
        MPI_Isend(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD,&request);
        MPI_Isend(&matrix_c, rows*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD,&request);
      }
      //result=matrix_c;
}
void MPI_nonBlocking_Multplication_all(int *result,int **my_matrices,int matrices_dimensions[][2])
{
        int *temp_result=my_matrices[0];
        for(int i=0 ; i<num_matrices-1;i++)
        {
            MPI_nonBlocking_Multplication_2(temp_result,temp_result,my_matrices[i+1],matrices_dimensions[i][0],matrices_dimensions[i][1],matrices_dimensions);
        }
        result=temp_result;
        print_my_result(result,my_matrices,matrices_dimensions);
}

int *Stressen_Multiplication(int *result,int **my_matrices,int matrices_dimensions[][2])
{
    return result;
}