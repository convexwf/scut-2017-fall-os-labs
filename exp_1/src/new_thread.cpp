#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

volatile int shared_var(0);
pthread_mutex_t mtx;

void* try_increase(void*)
{
	while(1)
	{
		pthread_mutex_lock(&mtx);
		++shared_var;
		pthread_mutex_unlock(&mtx);
	}
	pthread_exit(NULL);
}

void* print_variable(void*)
{
	while(1)
	{
		sleep(1);
		pthread_mutex_lock(&mtx);
		printf("Current shared_var : \t%d\n", shared_var);
		pthread_mutex_unlock(&mtx);
	}
}

int main(int argc, char** argv)
{
	pthread_mutex_init(&mtx, NULL);
	pthread_t th_inrease;
	pthread_t th_print;
	memset(&th_inrease, 0, sizeof(th_inrease));
	memset(&th_print, 0, sizeof(th_print));
	pthread_create(&th_inrease, NULL, try_increase, NULL);
	pthread_create(&th_print, NULL, print_variable, NULL);
	//pthread_join(th, NULL);
	while(1)
	{
		pthread_mutex_lock(&mtx);
		--shared_var;
		pthread_mutex_unlock(&mtx);
	}
	return 0;
}
