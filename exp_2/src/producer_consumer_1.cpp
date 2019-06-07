#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFERSIZE 5

struct id_producer{
	int id;
	char c;
};

volatile int currentSize(0);
volatile char BUFFER[BUFFERSIZE];
pthread_mutex_t mutex;

void* producer(void* args)
{
	id_producer pro = (id_producer)(*(id_producer*)args);
	int ID = pro.id;
	char item = pro.c;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		usleep(500000);
		printf("I am the producer %d CurrentSize = %d", ID, currentSize);
		if(currentSize < BUFFERSIZE )
		{
			printf("\tNow I push item %c into buffer\n", item);
			BUFFER[currentSize++] = item;
		}
		else
		{
			printf("\tRefuse to insert into buffer\n");
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

void* consumer(void* args)
{
	int ID = (int)(*(int*)args);
	char item;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		usleep(500000);
		printf("I am the consumer %d CurrentSize = %d", ID, currentSize);
		if(currentSize > 0)
		{
			item = BUFFER[--currentSize];
			printf("\tNow I take out %c into buffer\n", item);
		}
		else
		{
			printf("\tRefuse to take from buffer\n");
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv)
{
	pthread_mutex_init(&mutex, NULL);
	pthread_t threads[5];

	char Lower = 'q';
	char Super = 'Q';

	id_producer id_producer_1, id_producer_2;

	id_producer_1.id = 1;
	id_producer_1.c = Lower;
	id_producer_2.id = 2;
	id_producer_2.c = Super;
	int id_consumer_1 = 1;
	int id_consumer_2 = 2;
	int id_consumer_3 = 3;


	memset(threads, 0, sizeof(threads));
	pthread_create(&threads[0], NULL, producer, &id_producer_1);
	pthread_create(&threads[1], NULL, producer, &id_producer_2);
	pthread_create(&threads[2], NULL, consumer, &id_consumer_1);
	pthread_create(&threads[3], NULL, consumer, &id_consumer_2);
	pthread_create(&threads[4], NULL, consumer, &id_consumer_3);

	for(auto& th : threads) pthread_join(th, NULL);


	return 0;
}
