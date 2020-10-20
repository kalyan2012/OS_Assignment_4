# Q# Musical Mayhem

## Usage:
	input: k, a, e, c, t1, t2
	input: Next k lines have performer name instrument type time of arrival.

## function used:
	* void *p_func( void *z)
		This function implements the way of distribution threads.
	* int rndm(int a, int b)
		Returns random number between a, b including a, b.

## Explanation:
	* At first each thread sleeps for cretain time and then starts executing if a performer can perform only on a particular stage, the the semaphore related to that is applied wait.
	* For dual possible stages two threads must be created and find whichever thread executes first and must execute semaphore of that thread.
	* For singer the we have to check if a solo is available or duo on any of the stages and go to whichever is available first. We must lock the stage after singer enters so that 
	no other singer can enter
	* after, a semaphore for T - shirts distribution.

Unfortunately, even after logic I'm stuck at these 2 places ...
