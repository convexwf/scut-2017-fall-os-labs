#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "color.h"

#define chairs_all 10

sem_t empty_chair;
sem_t full_chair;
sem_t chairs_mutex;
sem_t cus_mutex;

volatile int chairs(chairs_all);

typedef struct Arg{
	int id;
} Arg;

void randomSleep(){
	int rd = rand()%100 + 1;
	usleep(rd * 10000);
    //sleep(1);
}

void* barber(void* args)
{
	Arg* arg = (Arg*)args;
	while(1)
	{
        printf(RED "I am the barber\tI am going to sleep\n" NONE);
        while(1)
        {
            sem_wait(&full_chair);
            sem_wait(&chairs_mutex);
            printf(RED "I am the barber. There are %d customers!\tI am going to work\n" NONE, chairs_all - chairs);
            ++chairs;
            sem_post(&chairs_mutex);
            sem_post(&empty_chair);
            if(chairs == chairs_all) break;
            randomSleep();
        }
	}
	pthread_exit(NULL);
}

void* customer(void* args)
{
	Arg* arg = (Arg*)args;
    sem_wait(&cus_mutex);
    sem_wait(&chairs_mutex);
    if(chairs == 0)
    {
        printf(GREEN "I am the custommer %d. There are %d chairs!\tI am going to leave\n" NONE, arg->id, chairs);
        sem_post(&cus_mutex);
        sem_post(&chairs_mutex);
        pthread_exit(NULL);
    }

    printf(GREEN "I am the custommer %d. There are %d chairs!\tI am going to sit\n" NONE, arg->id, chairs--);
    sem_post(&chairs_mutex);
    sem_post(&cus_mutex);
    sem_wait(&empty_chair);
    sem_post(&full_chair);
}

int main(int argc, char** argv)
{
    int count = 0;
    sem_init(&chairs_mutex, 0, 1);
    sem_init(&cus_mutex, 0, 1);
    sem_init(&full_chair, 0, 0);
    sem_init(&empty_chair, 0, chairs_all);
	pthread_t Barber;
    Arg barber_arg;
    barber_arg.id = count++;
    pthread_create(&Barber, NULL, barber, &barber_arg);

    while(1)
    {
        int t = rand()%10 + 1;
        pthread_t Custommer[10];
        Arg customer_arg[10];
        for(int i = 0; i < t; i++)
        {
            customer_arg[i].id = count++;
            pthread_create(&Custommer[i], NULL, customer, &customer_arg[i]);
        }
        randomSleep();
    }

}
