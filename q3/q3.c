#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
int k, a, e, c, t1, t2, t;
struct performer
{
	char name[100], instrument;
	int arrival_time, id, perf_time, stage_id, stage_type;
};
struct stage
{
	int type, cur_per;
	int cur_type[2];
	pthread_mutex_t lock;
};
struct stage st[1000];
struct performer per[1000];
pthread_t performer_thread[1000];
sem_t acoustic, electric, tshirts;
pthread_mutex_t zone[1000];
int check_first[1000], check_exit_first[1000], check_first_singer[1000], check_exit_first_singer[1000];
pthread_mutex_t  zone_singer[1000];
void *ac_time_singer(void *z), *el_time_singer(void *z);
int random_integer(int x, int y)
{
	int z = x+(rand()%(y-x+1));
	return z;
}
void *ac_time(void *z)
{
	int l = *(int *)z;
	struct timespec tm;
	if(clock_gettime(CLOCK_REALTIME, &tm) == -1)
		return NULL;
	tm.tv_sec+=t;
	int zooo;
	sem_getvalue(&acoustic, &zooo);
	int ret_val = sem_timedwait(&acoustic, &tm);
	if(per[l].stage_type != -1) {
		sem_post(&electric);
		return NULL;
	}
	sem_getvalue(&acoustic, &zooo);
	pthread_mutex_lock(&zone[l]);
	if(ret_val == -1)
	{
		if(check_exit_first[l] == -1) {
			check_exit_first[l] = 0;
			check_first[l] = 2;
			printf("\033[1;35m%s %c left because of impatience\033[0m\n", per[l].name, per[l].instrument);
			pthread_mutex_unlock(&zone[l]);
			return NULL;
		}
		sem_post(&acoustic);
		pthread_mutex_unlock(&zone[l]);
		return NULL;
	}
	if(check_first[l] == -1)
	{
		check_first[l] = 2;
		per[l].stage_type = 0;
		for(int i=0;i<a;i++)
		{
			pthread_mutex_lock(&st[i].lock);
			if(st[i].cur_per == 0)
			{
				st[i].cur_type[0] = l;
				per[l].stage_id = i;
				st[i].cur_per = 1;
				pthread_mutex_unlock(&st[i].lock);
				break;
			}
			else
				pthread_mutex_unlock(&st[i].lock);
		}
		int random_time = random_integer(t1, t2);
		per[l].perf_time = random_time;
		printf("\033[1;51m%s performing %c at acoustic stage for %d\033[0m\n",per[l].name, per[l].instrument, per[l].perf_time);
		sleep(per[l].perf_time);
		if(st[per[l].stage_id].cur_per == 2) {
			sleep(2);
			printf("\033[1;34m%s performance at acoustic stage finished\033[0m\n", per[l].name);
			printf("\033[1;34m%s performance at acoustic stage finished\033[0m\n", per[st[per[l].stage_id].cur_type[1]].name);
			st[per[l].stage_id].cur_per = 0;
			st[per[l].stage_id].cur_type[0] = -1;
			st[per[l].stage_id].cur_type[1] = -1;
		}
		else {
			printf("\033[1;51m%s performance at acoustic stage finished\033[0m\n", per[l].name);
			st[per[l].stage_id].cur_per--;
			st[per[l].stage_id].cur_type[0] = -1;
		}
	}
	pthread_mutex_unlock(&zone[l]);
	sem_post(&acoustic);
	return NULL;
}
void *el_time(void *z)
{
	int l = *(int *)z;
	struct timespec tm;
	if(clock_gettime(CLOCK_REALTIME, &tm) == -1)
		return NULL;
	tm.tv_sec+=t;
	int zooo;
	sem_getvalue(&electric, &zooo);
	int ret_val = sem_timedwait(&electric, &tm);
	if(per[l].stage_type != -1) {
		sem_post(&electric);
		return NULL;
	}
	sem_getvalue(&electric, &zooo);
	pthread_mutex_lock(&zone[l]);
	if(ret_val == -1)
	{
		if(check_exit_first[l] == -1) {
			check_exit_first[l] = 0;
			check_first[l] = 2;
			printf("\033[1;35m%s %c left because of impatience\033[0m\n", per[l].name, per[l].instrument);
			pthread_mutex_unlock(&zone[l]);
			return NULL;
		}
		pthread_mutex_unlock(&zone[l]);
		return NULL;
	}
	if(check_first[l] == -1)
	{
		check_first[l] = 2;
		per[l].stage_type = 1;
		for(int i=a;i<a+e;i++)
		{
			pthread_mutex_lock(&st[i].lock);
			if(st[i].cur_per == 0)
			{
				st[i].cur_type[0] = l;
				per[l].stage_id = i;
				st[i].cur_per = 1;
				pthread_mutex_unlock(&st[i].lock);
				break;
			}
			else
				pthread_mutex_unlock(&st[i].lock);
		}
		int random_time = random_integer(t1, t2);
		per[l].perf_time = random_time;
		printf("\033[1;57m%s performing %c at electric stage for %d\033[0m\n",per[l].name, per[l].instrument, per[l].perf_time);
		sleep(per[l].perf_time);
		if(st[per[l].stage_id].cur_per == 2) {
			sleep(2);
			printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[l].name);
			printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[st[per[l].stage_id].cur_type[1]].name);
			st[per[l].stage_id].cur_per = 0;
			st[per[l].stage_id].cur_type[0] = -1;
			st[per[l].stage_id].cur_type[1] = -1;
		}
		else {
			printf("\033[1;57m%s performance at electric stage finished\033[0m\n", per[l].name);
			st[per[l].stage_id].cur_per--;
			st[per[l].stage_id].cur_type[0] = -1;
		}
	}
	pthread_mutex_unlock(&zone[l]);
	sem_post(&electric);
	return NULL;
}
void *performance(void *z)
{
	int l = *(int *)z;
	sleep(per[l].arrival_time);
	printf("\033[1;32m%s %c arrived\033[0m\n", per[l].name, per[l].instrument);
	if(per[l].instrument == 'v')
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
		for(int i=0;i<a;i++)
		{
			pthread_mutex_lock(&st[i].lock);
			if(st[i].cur_per == 0)
			{
				st[i].cur_type[0] = l;
				per[l].stage_id = i;
				per[l].stage_type = 0;
				st[i].cur_per = 1;
				pthread_mutex_unlock(&st[i].lock);
				break;
			}
			else
				pthread_mutex_unlock(&st[i].lock);
		}
		int random_time = random_integer(t1, t2);
		per[l].perf_time = random_time;
		printf("\033[1;34m%s performing %c at acoustic stage for %d\033[0m\n",per[l].name, per[l].instrument, per[l].perf_time);
		sleep(per[l].perf_time);
		if(st[per[l].stage_id].cur_per == 2) {
			sleep(2);
			printf("\033[1;34m%s performance at acoustic stage finished\033[0m\n", per[l].name);
			printf("\033[1;34m%s performance at acoustic stage finished\033[0m\n", per[st[per[l].stage_id].cur_type[1]].name);
			st[per[l].stage_id].cur_per = 0;
			st[per[l].stage_id].cur_type[0] = -1;
			st[per[l].stage_id].cur_type[1] = -1;
		}
		else {
			printf("\033[1;34m%s performance at acoustic stage finished\033[0m\n", per[l].name);
			st[per[l].stage_id].cur_per--;
			st[per[l].stage_id].cur_type[0] = -1;
		}
	}
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
			printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[st[per[l].stage_id].cur_type[1]].name);
			st[per[l].stage_id].cur_per = 0;
			st[per[l].stage_id].cur_type[0] = -1;
			st[per[l].stage_id].cur_type[1] = -1;
		}
		else
			printf("\033[1;34m%s performance at electric stage finished\033[0m\n", per[l].name);
		st[per[l].stage_id].cur_per = 0;
		st[per[l].stage_id].cur_type[0] = -1;

	}
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
	else
	{
		int inside = 0, join = 0;
		for(int i=0;i<a+e;i++) {
			pthread_mutex_lock(&st[i].lock);
			if (st[i].cur_per == 0) {
				inside = 1;
				per[l].stage_id = i;
				per[l].stage_type = 1*(i >= a);
				st[i].cur_type[0] = l;
				st[i].cur_per = 1;
				int random_time = random_integer(t1, t2);
				per[l].perf_time = random_time;
				if(per[l].stage_type == 0)
					printf("\033[1;34m%s performing %c at acoustic stage for %d\033[0m\n",per[l].name, per[l].instrument, per[l].perf_time);
				else
					printf("\033[1;34m%s performing %c at electric stage for %d\033[0m\n",per[l].name, per[l].instrument, per[l].perf_time);
				sleep(per[l].perf_time);
				if(per[l].stage_type == 0)
					printf("\033[1;34m%s performance at acoustic stage finished\033[0m\n",per[l].name);
				else
					printf("\033[1;34m%s performance at electric stage finished\033[0m\n",per[l].name);
				st[per[l].stage_id].cur_per--;
				st[per[l].stage_id].cur_type[0] = -1;
				if (i >= a)
					sem_wait(&electric);
				else
					sem_wait(&acoustic);
				pthread_mutex_unlock(&st[i].lock);
				break;
			}
			else
				pthread_mutex_unlock(&st[i].lock);
		}
		if(!inside) {
			for (int i = 0; i < a + e; i++) {
				pthread_mutex_lock(&st[i].lock);
				if (st[i].cur_per == 1 && st[i].cur_type[0] != 's') {
					join = 1;
					per[l].stage_id = i;
					per[l].stage_type = st[i].type;
					st[i].cur_type[1] = l;
					st[i].cur_per = 2;
					per[l].perf_time = per[st[i].cur_type[0]].perf_time + 2;
					if(per[l].stage_type == 0)
						printf("\033[1;34m%s joined %s's performance on %d, performance extended for 2 sec on acoustic\033[0m\n",per[l].name, per[st[i].cur_type[0]].name,i);
					else
						printf("\033[1;34m%s joined %s's performance on %d, performance extended for 2 sec on electric\033[0m\n",per[l].name, per[st[i].cur_type[0]].name,i);
					sleep(per[l].perf_time);
					return NULL;
				}
			}
			if (!join) {
				pthread_t ac, el;
				check_exit_first_singer[l] = -1, check_first_singer[l] = -1;
				pthread_create(&ac, NULL, ac_time_singer, &(l));
				pthread_create(&el, NULL, el_time_singer, &(l));
				pthread_join(ac, NULL);
				pthread_join(el, NULL);
				check_first_singer[l] = -1, check_exit_first_singer[l] = -1;
			}
		}

	}
	sem_wait(&tshirts);
	sleep(2);
	sem_post(&tshirts);
	printf("%s has got T-shirt\n", per[l].name);
	return NULL;
}
int main()
{
	srand(time(0));
	scanf("%d %d %d %d %d %d %d", &k, &a, &e, &c, &t1, &t2, &t);
	sem_init(&acoustic, 0, a);
	sem_init(&electric, 0, e);
	sem_init(&tshirts, 0, c);
	for(int i=0;i<k;i++)
	{
		per[i].id = i;
		scanf("%s", per[i].name);
		getchar();
		scanf("%c", &per[i].instrument);
		scanf("%d", &per[i].arrival_time);
		per[i].perf_time = -1;
		per[i].stage_id = -1;
		per[i].stage_type = -1;
	}
	for(int i=0;i<a;i++)
	{
		st[i].type = 0;
		st[i].cur_per = 0;
		st[i].cur_type[0] = -1;
		st[i].cur_type[1] = -1;
	}
	for(int i=0;i<k;i++)
		pthread_create(&performer_thread[i], NULL, performance, &(per[i].id));
	for(int i=0;i<k;i++)
		pthread_join(performer_thread[i], NULL);
	for(int i=0; i<100;i++)
	{
		pthread_mutex_destroy(&zone[i]);
		pthread_mutex_destroy(&zone_singer[i]);
	}
	sem_destroy(&acoustic);
	sem_destroy(&electric);
	sem_destroy(&tshirts);
	printf("\n\033[1;31mFinished\n\033[0m");
}
void *ac_time_singer(void *z)
{
	int l = *(int *)z;
	struct timespec tm;
	if(clock_gettime(CLOCK_REALTIME, &tm) == -1)
		return NULL;
	tm.tv_sec+=t;
	int ret_val = sem_timedwait(&acoustic, &tm);
	if(per[l].stage_type != -1) {
		sem_post(&acoustic);
		return NULL;
	}
	pthread_mutex_lock(&zone_singer[l]);
	if(ret_val == -1)
	{
		if(check_exit_first[l] == -1) {
			printf("\033[1;35m%s %c left because of impatience\033[0m\n", per[l].name, per[l].instrument);
			check_exit_first[l] = 0;
			pthread_mutex_unlock(&zone_singer[l]);
			return NULL;
		}
		sem_post(&acoustic);
		pthread_mutex_unlock(&zone_singer[l]);
		return NULL;
	}
	if(check_first_singer[l] == -1)
	{
		check_first_singer[l] = 2;
		per[l].stage_type = 0;
		for(int i=0;i<a;i++)
		{
			pthread_mutex_lock(&st[i].lock);
			if(st[i].cur_per == 0)
			{
				st[i].cur_type[0] = l;
				per[l].stage_id = i;
				st[i].cur_per = 1;
				per[l].perf_time = random_integer(t1, t2);
				pthread_mutex_unlock(&st[i].lock);
				break;
			}
			else
				pthread_mutex_unlock(&st[i].lock);
		}
		sleep(per[l].perf_time);
		printf("\033[1;51m%s performance at acoustic stage finished\033[0m\n",per[l].name);
	}
	st[per[l].stage_id].cur_per--;
	st[per[l].stage_id].cur_type[0] = -1;
	sem_post(&acoustic);
	pthread_mutex_unlock(&zone_singer[l]);
	return NULL;
}
void *el_time_singer(void *z)
{
	int l = *(int *)z;
	struct timespec tm;
	if(clock_gettime(CLOCK_REALTIME, &tm) == -1)
		return NULL;
	tm.tv_sec+=t;
	int ret_val = sem_timedwait(&electric, &tm);
	if(per[l].stage_type != -1) {
		sem_post(&electric);
		return NULL;
	}
	pthread_mutex_lock(&zone_singer[l]);
	if(ret_val == -1)
	{
		if(check_exit_first_singer[l] == -1) {
			printf("\033[1;35m%s %c left because of impatience\033[0m\n", per[l].name, per[l].instrument);
			check_exit_first_singer[l] = 0;
			pthread_mutex_unlock(&zone_singer[l]);
			return NULL;
		}
		sem_post(&electric);
		pthread_mutex_unlock(&zone_singer[l]);
		return NULL;
	}
	if(check_first_singer[l] == -1)
	{
		check_first_singer[l] = 2;
		per[l].stage_type = 1;
		for(int i=0;i<a;i++)
		{
			pthread_mutex_lock(&st[i].lock);
			if(st[i].cur_per == 0)
			{
				st[i].cur_type[0] = l;
				per[l].stage_id = i;
				st[i].cur_per = 2;
				per[l].perf_time = random_integer(t1, t2);
				pthread_mutex_unlock(&st[i].lock);
				break;
			}
			else
				pthread_mutex_unlock(&st[i].lock);
		}
		sleep(per[l].perf_time);
		printf("\033[1;57m%s performance at electric stage finished\033[0m\n",per[l].name);
	}
	sem_post(&electric);
	st[per[l].stage_id].cur_per--;
	st[per[l].stage_id].cur_type[0] = -1;
	pthread_mutex_unlock(&zone_singer[l]);
	return NULL;
}
