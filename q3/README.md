# Q# Musical Mayhem

## Usage:
	input: k, a, e, c, t1, t2
	input: Next k lines have performer name instrument type time of arrival.

## function used:
	* void *perform(void *z)
		This function implements the way of distribution threads.
	* void *ac_time(void *z)
		This runs the acoustic thread of a piano or guitar player
	* void *el_time(void *z)
		This runs the electric thread of a piano or guitar player	
	* void *ac_time_singer(void *z)
		This runs the acoustic thread of a singer if no join or single is available.
	* void *el_time_singer(void *z)
		This runs the electric thread of a singer if no join or single is available.
	* int random_integer(int a, int b)
		Returns random number between a, b including a, b.

## Explanation:
	* At first each thread sleeps for cretain time and then starts executing.
		
		int l = *(int *)z;
    			sleep(per[l].arrival_time);

	* semaphores for each acoustic and electric stages are created.
		
		sem_init(&acoustic, 0, a);
    		sem_init(&electric, 0, e);
    		sem_init(&tshirts, 0, c);
	
	* if a performer is either voilin or bass he just need to use sem_timedwait() and exit if he didnot get the stage.
		
if(per[l].instrument == 'b')
    {
        struct timespec tm;
        if(clock_gettime(CLOCK_REALTIME, &tm) == -1)
            return NULL;
        tm.tv_sec += t;
        int ret = 0;
        ret = sem_timedwait(&acoustic, &tm);
        if(ret == -1)
        {
            printf("%s left because of impatience\n", per[l].name);
            return NULL;
        }
        for(int i=a;i<a+e;i++)
        {
            pthread_mutex_lock(&st[i].lock);
            if(st[i].cur_per == 0)
            {
                st[i].cur_type[0] = l;
                per[l].stage_id = i;
                per[l].stage_type = 1;
                st[i].cur_per = 1;
                pthread_mutex_unlock(&st[i].lock);
                break;
            }
            else
                pthread_mutex_unlock(&st[i].lock);
        }
        int random_time = random_integer(t1, t2);
        per[l].perf_time = random_time;
        printf("\033[1;34m%s performing %c at electric stage for %d\033[0m\n",per[l].name, per[l].instrument, per[l].perf_time);
        sleep(per[l].perf_time);
        if(st[per[l].stage_id].cur_per == 2) {
            sleep(2);
            printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[l].name);
            printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[l].name);
            st[per[l].stage_id].cur_per = 0;
            st[per[l].stage_id].cur_type[0] = -1;
            st[per[l].stage_id].cur_type[1] = -1;
        }
        else
            printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[l].name);
            st[per[l].stage_id].cur_per = 0;
            st[per[l].stage_id].cur_type[0] = -1;

    }
	* If he is a piano player or guitar player 2 threads are initialised to keep trak of both stage positons and go to stage that is first available.
 
else if(per[l].instrument == 'p' || per[l].instrument == 'g')
    {
        pthread_t ac, el  ;
        check_exit_first[l] = -1, check_first[l] = -1;
        pthread_create(&ac, NULL, ac_time, &(l));
        pthread_create(&el, NULL, el_time, &(l));
        pthread_join(ac, NULL);
        pthread_join(el, NULL);
        check_first[l] = -1, check_exit_first[l] = -1;
    }

	* If he is a singer he can choose a empty stage or a join stage if there is a musician.
	* If no stage is available for join also then he must wait sem_timedwait() for getting a free stage orelse he exits.
	* after, a semaphore for T - shirts distribution.
	
	sem_wait(&tshirts);
    	sleep(2);
    	sem_post(&tshirts);
