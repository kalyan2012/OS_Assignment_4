#define _POSIX_C_SOURCE 199309L
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

int * shareMem(size_t size)
{
	key_t mem_key = IPC_PRIVATE;
	int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
	return (int*)shmat(shm_id, NULL, 0);
}
int swap(int *a, int *b)
{
	int temp = *a;
	*b = *a;
	*a = temp;
}
void selection_sort(int arr[], int l, int r)
{
	int c, d, mi, n=r-l+1;
	for (c = 0; c < n-1; c++)
	{
		mi = c;
		for (d = c+1; d < n; d++)
			if (arr[d] < arr[mi])
				mi = d;
		swap(&arr[mi], &arr[c]);
	}
}
void merge (int *arr, int low, int mid, int high)
{
		int size1 = mid - low + 1;
		int size2 = high - mid;
		int L[size1], R[size2];
		for (int j = 0; j < size1; j++)
			L[j] = arr[low + j];
		for (int j = 0; j < size2; j++)
			R[j] = arr[mid + j + 1];
		int i = 0, j = 0, k = low;
		while (i < size1 && j < size2) 
		{
			if (L[i] <= R[j]) 
			{
				arr[k] = L[i];
				i++;
			} 
			else 
			{
				arr[k] = R[j];
				j++;
			}
			k++;
		}
		while (i < size1) 
		{
			arr[k] = L[i];
			i++;
			k++;
		}
		while (j < size2) 
		{
			arr[k] = R[j];
			j++;
			k++;
		}
}
void normal_mergesort(int *arr, int low, int high)
{
	if(low<high)
	{
		if(high-low + 1 < 5)
		{
			selection_sort(arr, low, high);
			return;
		}
		int mid = low + (high-low)/2;
		normal_mergesort(arr, low, mid);
		normal_mergesort(arr, mid + 1, high);
		merge(arr, low, mid, high);
	}
}
void mergesort(int *arr, int low, int high)
{
	if(low<high)
	{
		if(high-low + 1 < 5)
		{
			selection_sort(arr, low, high);
			return;
		}
		int mid = low + (high-low)/2;
		int pid1=fork();
		int pid2;
		if(pid1==0)
		{
			mergesort(arr, low, mid);
			_exit(1);
		}
		else
		{
			pid2=fork();
			if(pid2==0)
			{
				mergesort(arr, mid + 1, high);
				_exit(1);
			}
			else
			{
				int status;
				waitpid(pid1, &status, 0);
				waitpid(pid2, &status, 0);
				merge(arr, low, mid, high);
			}
		}
	}
}
struct arg
{
	int l;
	int r;
	int* arr;
};
void *threaded_mergesort(void* a)
{
	struct arg *args = (struct arg*) a;
	int l = args->l;
	int r = args->r;
	int *arr = args->arr;
	if(l>r) return NULL;
	if(r-l+1 < 5)
	{
		selection_sort(arr, l, r);
		return NULL;
	}	
	int mid = l+(r-l)/2;
	struct arg a1, a2;
	a1.l = l;
	a1.r = mid;
	a1.arr = arr;
	pthread_t tid1;
	pthread_create(&tid1, NULL, threaded_mergesort, &a1);
	a2.l = mid+1;
	a2.r = r;
	a2.arr = arr;
	pthread_t tid2;
	pthread_create(&tid2, NULL, threaded_mergesort, &a2);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	merge(arr,l, mid, r);
}
void runSorts(int n)
{
	struct timespec ts;
	int *arr = shareMem(sizeof(int)*(n+1));
	for(int i=0;i<n;i++)
		scanf("%d", arr+i);
	int brr[n+1], brr1[n+1];
	for(int i=0;i<n;i++)
	{
		brr[i] = arr[i];
		brr1[i] = arr[i];
	}
	printf("\n");
	//For forked mergesort
	printf("Running concurrent_mergesort for n = %d\n", n);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
	mergesort(arr, 0, n-1);
	for(int i=0; i<n; i++)
		printf("%d ",arr[i]);
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n\n", en - st);
	long double t1 = en-st;
	// For threaded mergesort
	pthread_t tid;
	struct arg a;
	a.l = 0;
	a.r = n-1;
	a.arr = brr;
	printf("Running threaded_concurrent_mergesort for n = %d\n", n);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;
	pthread_create(&tid, NULL, threaded_mergesort, &a);
	pthread_join(tid, NULL);
	for(int i=0; i<n; i++)
		printf("%d ",a.arr[i]);
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n\n", en - st);
	long double t2 = en-st;
	// Normalmergesort
	printf("Running normal_mergesort for n = %d\n", n);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	st = ts.tv_nsec/(1e9)+ts.tv_sec;
	normal_mergesort(brr1, 0, n-1);
	for(int i=0; i<n; i++)
		printf("%d ",brr1[i]);
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	en = ts.tv_nsec/(1e9)+ts.tv_sec;
	printf("time = %Lf\n\n", en - st);
	long double t3 = en - st;
	printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n", t1/t3, t2/t3);
	shmdt(arr);
}
int main()
{
	int n;
	scanf("%d", &n);
	runSorts(n);
	return 0;
}
