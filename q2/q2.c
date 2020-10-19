#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
int n, m, o, Roaming;
pthread_mutex_t mutex[1000], mutex_company[1000];
int array[1000];
struct Struct_Student
{
	int id, round_no, is_cured;
};
struct Struct_Company
{
	int id;
	int batches_dirtibued;
	float p;
};
struct Struct_Zone
{
	int id, Company_id, vaccine_left, in_vacc, slots, queue[10], no_of_st;
};
int min(int a, int b, int c)
{
	return (a<b?(a>c?c:a):(b>c?c:b));
}
int random(int low, int high)
{
	// Both bounds inclusive
	return (low+rand()%(high - low + 1));
}
void *c_func(), *v_func(), *s_func();
struct Struct_Company Company[1000];
struct Struct_Zone Zone[1000];
struct Struct_Student Student[1000];
int main()
{
	srand(time(0));
	scanf("%d %d %d", &n, &m, &Roaming);
	int total_attended = Roaming;
	o=0;
	if(n == 0)
	{
		printf("\033[1;34mSorry there are no sponsering companies\033[0m\n");
		printf("\033[1;33mSimulation Over\033[0;m\n");
		exit(0);
	}
	if(m == 0)
	{
		printf("\033[1;34Sorry no Zones are avaiable to provide you with vaccine\033[0m\n");
		printf("\033[1;35mSimulation Over\033[0;m\n");
		exit(0);
	}
	float a[n];
	for(int i=0;i<n;i++)
		scanf("%f", &a[i]);
	sleep(2);
	pthread_t company_thread[n], vaccine_zone_thread[m], student_thread[o];
	for(int i=0;i<n;i++)
	{
		Company[i].id = i;Company[i].p = a[i];
		Company[i].batches_dirtibued = 0;
		pthread_create(&company_thread[i], NULL, c_func, &(Company[i].id));
	}
	for(int i=0;i<m;i++)
	{
		Zone[i].id = i;Zone[i].Company_id = -1;
		Zone[i].vaccine_left = 0;Zone[i].in_vacc = 0;
		Zone[i].no_of_st = 0;Zone[i].slots = 0;
		pthread_mutex_init(&mutex[i], NULL);
		pthread_mutex_init(&mutex_company[i], NULL);
		pthread_create(&vaccine_zone_thread[i], NULL, v_func, &(Zone[i].id));
	}
	for(int i=0;i<Roaming;i++)
	{
		Student[i].id = i;
		Student[i].round_no = 0;
		Student[i].is_cured = -1;
		pthread_create(&student_thread[i], NULL, s_func, &(Student[i].id));
	}
	for(int i=0;i<total_attended;i++)
		pthread_join(student_thread[i], NULL);
	for(int i=0;i<m;i++)
	{
        pthread_mutex_destroy(&mutex[i]);
        pthread_mutex_destroy(&mutex_company[i]);
    }
	printf("\n\033[1;31mSimluation Over\033[0m\n");
	return 0;
}
void *c_func(void *a)
{
	int z = *(int *)a;
	while(Roaming)
	{
		while(Company[z].batches_dirtibued);
		printf("\033[1;36mAll the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now\033[0m\n", z);
		sleep(1);
		int w = random(2, 5), r = random(1, 5), p = random(10, 20);
		printf("\033[1;34mPharmaceutical Company %d is preparing %d batches of vaccines which have success probability %f\033[0m\n",z, r, Company[z].p);
		sleep(w);
		for(int i=0;i<r;i++)
		{
			while(1)
			{
				int supply_zone = random(0,m-1);
				pthread_mutex_lock(&mutex_company[supply_zone]);
				if(Zone[supply_zone].vaccine_left == 0)
				{
					Company[z].batches_dirtibued++;
					printf("\033[1;35mPharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %f\033[0m\n", z, supply_zone, Company[z].p);
					sleep(1);
					Zone[supply_zone].vaccine_left = p;
					Zone[supply_zone].Company_id = z;
					printf("\033[1;33mPharmaceutical Company %d has delivered vaccines to Vaccination Zone %d, resuming vaccinations\033[0m\n", z, supply_zone);
					sleep(1);
					pthread_mutex_unlock(&mutex_company[supply_zone]);
					break;
				}
				else
					pthread_mutex_unlock(&mutex_company[supply_zone]);
			}
		}
	}
}
void *v_func(void *a)
{
	while(Roaming) 
	{
		int z = *(int *) a;
		while(!Zone[z].vaccine_left);
		Zone[z].slots = min(8, Zone[z].vaccine_left, o);
		printf("Vaccine Zone %d has got %d vaccines\n", z, Zone[z].vaccine_left);
		if (Zone[z].slots)
		{
			printf("\033[1;34mVaccination Zone %d is ready to vaccinate with %d slots\033[0m\n", z, Zone[z].slots);
			sleep(1);
		}
		while ((Zone[z].slots!=Zone[z].no_of_st && o) || !Zone[z].no_of_st);
		printf("\033[1;34mVaccination Zone %d is entering Vaccination Phase\033[0m\n", z);
		sleep(1);
		Zone[z].in_vacc = 1;
		for (int i = 0; i < Zone[z].no_of_st; i++) 
		{
			printf("\033[1;35mStudent %d on Vaccination Zone %d has been vaccinated which has success probability %f\033[0m\n",Zone[z].queue[i], z, Company[Zone[z].Company_id].p);
			float probability = ((float) rand()) / RAND_MAX;
			if (probability <= Company[Zone[z].Company_id].p)
				Student[Zone[z].queue[i]].is_cured = 1;
			else 
			{
				Student[Zone[z].queue[i]].is_cured = 0;
				if (Student[Zone[z].queue[i]].round_no != 3)
					o++;
			}
			sleep(2);
		}
		Zone[z].vaccine_left = Zone[z].vaccine_left - Zone[z].no_of_st;
		if (Zone[z].vaccine_left == 0) 
		{
			printf("\033[01;33mVaccination Zone %d has run out of vaccines\033[0m\n", z);
			Company[Zone[z].Company_id].batches_dirtibued--;
			sleep(1);
			while(!Zone[z].vaccine_left);
		}
		Zone[z].slots = 0;Zone[z].no_of_st= 0;Zone[z].in_vacc = 0;
	}
}
void *s_func(void *a)
{
	int z = *(int *)a;
	int rndm = random(5, 10);
	//printf("Roaming by student %d for time %d\n", z, rndm);
	sleep(rndm);
	o++;
	while(Student[z].round_no < 3)
	{
		for(int i=0;i<m;i++)
	    	while(!Zone[i].vaccine_left);
		sleep(1);
		printf("\033[1;31mStudent %d is waiting to be allocated a slot on a Vaccination Zone\033[0m\n", z);
		int zone_allotted;
		while(1)
		{
			zone_allotted = random(0, m-1);
			//printf("before of mutex lock by %d\n", z);
			pthread_mutex_lock(&(mutex[zone_allotted]));
			if(!Zone[zone_allotted].in_vacc && Zone[zone_allotted].no_of_st!=Zone[zone_allotted].slots)
			{
				Zone[zone_allotted].no_of_st++;
				o--;
			//	printf("In lock by %d\n", z);
				Zone[zone_allotted].queue[Zone[zone_allotted].no_of_st-1] = Student[z].id;	
				Student[z].round_no++;
				if(Student[z].round_no == 1)
					printf("\033[1;34mStudent %d has arrived for his 1st round of Vaccination\033[0m\n", z);
				else if(Student[z].round_no == 2)
					printf("\033[1;34mStudent %d has arrived for his 2nd round of Vaccination\033[0m\n", z);
				else if(Student[z].round_no == 3)
					printf("\033[1;34mStudent %d has arrived for his 3rd round of Vaccination\033[0m\n", z);	
				printf("\033[1;32mStudent %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\033[0m\n", z, zone_allotted );
				pthread_mutex_unlock(&(mutex[zone_allotted]));
				break;
			}
			else
				pthread_mutex_unlock(&(mutex[zone_allotted]));
			//printf("after of lock %d\n",z );
		}
		sleep(2);
		while(Student[z].is_cured == -1);
		sleep(1);
		if(Student[z].is_cured == 0)
		{
			printf("\033[1;35mStudent %d has tested negative for antibodies\033[0m\n", z);	
			if(Student[z].round_no != 3)
				printf("Student %d must have to go through again\n", z);
			else
				printf("Sorry Student %d you have to leave home for your safety!!!\n", z);
			Student[z].is_cured = -1;
		}
		else if(Student[z].is_cured == 1)
		{
			printf("\033[1;35mStudent %d has tested positive for antibodies\033[0m\n", z);
			printf("Student %d is free to enter the college\n", z);
			break;
		}
	}
	//printf("Student %d\n", z);
}
