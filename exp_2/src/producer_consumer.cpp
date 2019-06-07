#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "color.h"

#define MAX_SUM 	5
#define MAX_UPPER 	5
#define MAX_LOWER 	5

sem_t all_empty;
sem_t all_full;
sem_t lower_empty;
sem_t lower_full;
sem_t upper_empty;
sem_t upper_full;

volatile int upper_buffer[MAX_UPPER];
volatile int lower_buffer[MAX_LOWER];
volatile int upper_index(0);
volatile int lower_index(0);

const char upper_case = 'Q';
const char lower_case = 'q';

struct role{
	char* status;
	int id;
	int type;	// upper_case 1; lower_case -1; random 0;
};

void randomSleep()
{
	int rd = rand()%100 + 1;
	usleep(rd * 10000);
}

void* Producer(void* args)
{
	role* producer = (role*) args;
	while(1)
	{
		switch(producer->type)
		{
		case 1:
			sem_wait(&all_empty);
			sem_wait(&upper_empty);
			printf(RED "I am the %s %d\t Now upper:%d lower: %d\tSo I push item %c into buffer\n" NONE,
			producer->status, producer->id, upper_index, lower_index, upper_case);
			upper_buffer[upper_index++] = upper_case;
			sem_post(&upper_full);
			sem_post(&all_full);

			break;

		case -1:

			sem_wait(&all_empty);
			sem_wait(&lower_empty);
			printf(RED "I am the %s %d\t Now upper:%d lower: %d\tSo I push item %c into buffer\n" NONE,
			producer->status, producer->id, upper_index, lower_index, lower_case);
			lower_buffer[lower_index++] = lower_case;
			sem_post(&lower_full);
			sem_post(&all_full);

			break;

		default:
			break;
		}
		randomSleep();
	}
	pthread_exit(NULL);
}

void* Consumer(void* args)
{
	role* consumer = (role*) args;
	while(1)
	{
		switch(consumer->type)
		{
		case 1:
			sem_wait(&upper_full);
			sem_wait(&all_full);
			printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
			consumer->status, consumer->id, upper_index, lower_index, upper_case);
			--upper_index;
			sem_post(&upper_empty);
			sem_post(&all_empty);

			break;

		case -1:
			sem_wait(&lower_full);
			sem_wait(&all_full);
			printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
			consumer->status, consumer->id, upper_index, lower_index, lower_case);
			--lower_index;
			sem_post(&lower_empty);
			sem_post(&all_empty);

			break;

		case 0:
			if(upper_index >= lower_index)
			{
				sem_wait(&upper_full);
				sem_wait(&all_full);
				printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
				consumer->status, consumer->id, upper_index, lower_index, upper_case);
				--upper_index;
				sem_post(&upper_empty);
				sem_post(&all_empty);
			}
			else
			{
				sem_wait(&lower_full);
				sem_wait(&all_full);
				printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
				consumer->status, consumer->id, upper_index, lower_index, lower_case);
				--lower_index;
				sem_post(&lower_empty);
				sem_post(&all_empty);
			}

			break;

		default:
			break;
		}
		randomSleep();
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	pthread_t threads[5];
	memset(threads, 0, sizeof(threads));
	role roles[5];

	sem_init(&upper_full, 0, 0);
    sem_init(&upper_empty, 0, MAX_UPPER);
	sem_init(&lower_full, 0, 0);
    sem_init(&lower_empty, 0, MAX_LOWER);
	sem_init(&all_full, 0, 0);
    sem_init(&all_empty, 0, MAX_SUM);

	roles[0].status = "producer";
	roles[0].id = -1;
	roles[0].type = -1;
	roles[1].status = "producer";
	roles[1].id = 1;
	roles[1].type = 1;
	roles[2].status = "consumer";
	roles[2].id = -1;
	roles[2].type = -1;
	roles[3].status = "consumer";
	roles[3].id = 0;
	roles[3].type = 0;
	roles[4].status = "consumer";
	roles[4].id = 1;
	roles[4].type = 1;

	pthread_create(&threads[0], NULL, Producer, &roles[0]);
	pthread_create(&threads[1], NULL, Producer, &roles[1]);
	pthread_create(&threads[2], NULL, Consumer, &roles[2]);
	pthread_create(&threads[3], NULL, Consumer, &roles[3]);
	pthread_create(&threads[4], NULL, Consumer, &roles[4]);

	for(auto& th : threads) pthread_join(th, NULL);
}
