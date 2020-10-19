# Implementation of Merge Sort

## Usage

Enter following arguments:

	n
	[N integers]

##Functions used:

* To create shared memory:
	int *shareMem(size_t size)

* For Multiprocess Mergesort:
	void mergesort(int *arr, int l, int r)

* Threaded Mergesort:
	void *threaded_mergesort(void *a)

* Dividing function:
	normal_mergesort(int *arr, int low, int high) 

* Selection sort:
	Selection_sort(int *arr, int low, int high)

* swapping:
	swap(int *a, int *b)

* Mergeing divded parts:
	merge(int *arr, int low, int mid, int high)

## What happens:
	
	The program takes n as input and followed by n integers.
	First it is sorted using multi-process mergesort.
	Next it is implemented using multi-threaded mergesort.
	After normal mergesort code is implemented.
* It is observed that Threaded mergesort is faster than that of forked one.

## Explanation:
	
	* For multi-threaded mergesort;
		Array is divided into two halves where each half is sorted on two different threads, and thus formed arrays undergo recursively like this.
		If the size of array is less than 5 then selection sort is implemented.
	* For multi-process mergesort:
		Array is divided into two halves where each half is sorted on two different process, and thus formed arrays undergo recursively like this.
		If the size of array is less than 5 then selection sort is implemented.

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

## Comparision between Multi-threaded and Multi-process mergesorts:
	
	* As multi-process mergesort creates two process by two forks, after each fork completes execution then Parent is resumed and merges the results of the two forks.
	* As multi-threaded mergesort creates two threads, after each thread completes execution then Parent thread is resumed and merges the results of the two threads.

	The main reason for multi-threaded mergesort taking less time than that of multi-process mergesort is that for a process the processor has to save the entire contents of present 
	process and restart a new process and retreive them after completion. But for threads they share the data used by the main process they just run with some extra attributes and 
	share a commaon memory which saves processor to store the contents regulary. Also the context switch time for processes are greater than that of threads for a process.
	
	Thus threaded mergesort is faster than Multi-process mergesort..

## Comparision between Multi-threaded and Normal mergesorts:
	
	* A multi-threaded mergesort creates nearly 2 power n-1 threads but threading only benifits if the threads created are nearly equal to number of cores of the computer
	giving a mere 2 to 4 integers for sorting to a thread is less work, this overhead of the creation of threads for this purpose far outweighs the gain of paralellism.
	
	So, Normal mergesort is faster than Multi-threaded mergesort.
	
## Comparision between Multi-process and Normal mergesorts:
	* Generally, a process implemented using threads is faster than implementing using processes. Thus, due to creation of many forks and saving their values for context switches makes 
	the multi-process mergesort slower.

	So, Normal mergesort is faster than Multi-process mergesort.
