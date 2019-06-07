#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define BUFFERSIZE 5
#define PRODUCER_NUM 2
#define CONSUMER_NUM 3

sem_t empty;
sem_t full;

volatile int currentSize(0);
volatile char BUFFER[BUFFERSIZE];

struct arg{
	char* sign;
	int id;
	char c;
};

void randomSleep()
{
	int rd = rand()%100 + 1;
	usleep(rd * 10000);
}

void* producer(void* args)
{
	arg* _args = (arg*)args;
	while(1)
	{
		sem_wait(&empty);
		printf("I am the %s %d CurrentSize = %d\tSo I push item %c into buffer\n", _args->sign, _args->id, currentSize, _args->c);
		BUFFER[currentSize++] = _args->c;
		sem_post(&full);

		randomSleep();
	}
	pthread_exit(NULL);
}

void* consumer(void* args)
{
	arg* _args = (arg*)args;
	while(1)
	{
		sem_wait(&full);
		printf("I am the %s %d CurrentSize = %d\tSo I take item %c from buffer\n", _args->sign, _args->id, currentSize, _args->c);
		_args->c = BUFFER[--currentSize];
		sem_post(&empty);

		randomSleep();
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	pthread_t threads[5];
	memset(threads, 0, sizeof(threads));
	arg args[5];

	char lowerCase = 'q';
	char superCase = 'Q';

	sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFERSIZE);

	args[0].sign = "producer";
	args[0].id = 1;
	args[0].c = lowerCase;
	args[1].sign = "producer";
	args[1].id = 2;
	args[1].c = superCase;
	args[2].sign = "consumer";
	args[2].id = 1;
	args[2].c = NULL;
	args[3].sign = "consumer";
	args[3].id = 2;
	args[3].c = NULL;
	args[4].sign = "consumer";
	args[4].id = 3;
	args[4].c = NULL;

	pthread_create(&threads[0], NULL, producer, &args[0]);
	pthread_create(&threads[1], NULL, producer, &args[1]);
	pthread_create(&threads[2], NULL, consumer, &args[2]);
	pthread_create(&threads[3], NULL, consumer, &args[3]);
	pthread_create(&threads[4], NULL, consumer, &args[4]);

	for(auto& th : threads) pthread_join(th, NULL);
}
