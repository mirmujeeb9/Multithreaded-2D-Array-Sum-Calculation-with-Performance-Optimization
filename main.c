#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include<cmath>

#include<sys/time.h>
#define dimension_array 16
#define span 8000
#define tally_thread 4

int array[dimension_array][dimension_array];
pthread_mutex_t sum_mutex;
long long total = 0;


//function which calculates sum of array by using cyclic division for data
void *sum_versioncyclic(void *arg) {
int id_thrd = *((int *)arg);
long long thread_sum = 0;
 for (int i = id_thrd * dimension_array / tally_thread; i < (id_thrd + 1) * 		dimension_array / tally_thread; ++i) 
   {
	for (int j = 0; j < dimension_array; ++j) 
      {
	   thread_sum += array[i][j];
      }
   }
pthread_mutex_lock(&sum_mutex);
total += thread_sum;
pthread_mutex_unlock(&sum_mutex);
return NULL;
}


//function which calculates sum of array by using blockwise distribution for data
void *blockversion(void *arg) {
int sizeofblock = *((int *)arg);
int i, j, k, l;
  for (i = 0; i < dimension_array; i += sizeofblock) 
   {  
   for (j = 0; j < dimension_array; j += sizeofblock) 
       {
	long long block_sum = 0;
	  for (k = i; k < i + sizeofblock && k < dimension_array; ++k)
             {
	     for (l = j; l < j + sizeofblock && l < dimension_array; ++l) 
                {
	        block_sum += array[k][l];
	     }
                }
	pthread_mutex_lock(&sum_mutex);
	total += block_sum;
	pthread_mutex_unlock(&sum_mutex);
       }
   }
return NULL;
}


int main() {
int i, j;
int operation_count=(pow(2,16)*3) + (pow(2,16)*4);
int sizeofblock[] = {2, 4, 8};
double timetaken=0;
double gflops=0;
struct timeval start,end;
int thread_ids[] = {0, 1, 2, 3};
pthread_t threads[tally_thread];
  clock_t initial_time, final_time;
   //populating the array using rand function
   srand(time(NULL));
    for (int i = 0; i < dimension_array; i++)
    {
        for (int j = 0; j < dimension_array; j++)
        {
            array[i][j] = rand() % (16000 - 8000 + 1) + 8000;

        }
    }

printf("Block Wise: \n");
    //for loop to calcualte the total using the block size provide
  //while (i<sizeof(sizeofblock) / sizeof(sizeofblock[0])){
    for (i = 0; i < sizeof(sizeofblock) / sizeof(sizeofblock[0]); ++i) {
	total = 0;
	initial_time = clock();
	gettimeofday(&start,NULL);
	//creating thread and joining them for calculation
	pthread_create(&threads[0], NULL, blockversion, &sizeofblock[i]);
	pthread_join(threads[0], NULL);
	//noting the final time 
	gettimeofday(&end,NULL);
	final_time = clock();
	printf("Block size %d: Sum = %lld\n", sizeofblock[i], total);
	double seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
       double Gflops = 2e-9*operation_count/seconds;
	timetaken = (double)(final_time - initial_time) / CLOCKS_PER_SEC;
	gflops = 2 * dimension_array * dimension_array * 1e-9 /timetaken;
	printf("total time taken for the task computation is  %f\n", timetaken);
        printf("calculation of gflops is: %f\n\n", gflops);

}
total=0;
//printf("total is %lld\n",total);


//cyclic version part is below
initial_time = clock();

printf("\nCyclic version of task: \n");
  //creating threads first
  for (i = 0; i < tally_thread; ++i) {
	total = 0;
	pthread_create(&threads[i], NULL, sum_versioncyclic, &thread_ids[i]);
	}
  //joining threads
  for (i = 0; i < tally_thread; ++i) {
	pthread_join(threads[i], NULL);
	}

final_time = clock();
printf("Calculated total is :  = %lld\n", total);
timetaken = (double)(final_time - initial_time) / CLOCKS_PER_SEC;
gflops = 2 * dimension_array * dimension_array * 1e-9 / timetaken;
printf("total time taken for the task computation is : %f\n", timetaken);
printf("calculation of gflops is :  %f\n", gflops);

return 0;
}
