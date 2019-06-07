#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include "color.h"
#include "sem_p.h"
#include <unistd.h>

#define MAX_SUM 	5
#define MAX_UPPER 	5
#define MAX_LOWER 	5

void randomSleep(){
	int rd = rand()%100 + 1;
	usleep(rd * 10000);
    //sleep(1);
}
const char upper_case = 'Q';
const char lower_case = 'q';

struct shared_use_st  {
    int processID;
	int upper_buffer[MAX_UPPER];
	int lower_buffer[MAX_LOWER];
	int upper_index;
	int lower_index;
};

enum sem_id{
    all_empty = 0,
    all_full = 1,
    lower_empty = 2,
    lower_full = 3,
    upper_empty = 4,
    upper_full = 5,
    sem_process = 6
};

struct shared_use_st* shared;
int sem_proc;

struct role{
	char* status;
	int id;
	int type;	// upper_case 1; lower_case -1; random 0;
};



void* Producer(void* args){
	role* producer = (role*) args;
	while(1)
	{
		switch(producer->type)
		{
		case 1:
			down(sem_proc, all_empty);   //sem_wait(&all_empty);
			down(sem_proc, upper_empty); //sem_wait(&upper_empty);
			printf(RED "I am the %s %d\t Now upper:%d lower: %d\tSo I push item %c into buffer\n" NONE,
			producer->status, producer->id, shared->upper_index, shared->lower_index, upper_case);
			shared->upper_buffer[shared->upper_index++] = upper_case;
			up(sem_proc, upper_full);//sem_post(&upper_full);
			up(sem_proc, all_full);//sem_post(&all_full);

			break;

		case -1:

			down(sem_proc, all_empty);//sem_wait(&all_empty);
			down(sem_proc, lower_empty);//sem_wait(&lower_empty);
			printf(RED "I am the %s %d\t Now upper:%d lower: %d\tSo I push item %c into buffer\n" NONE,
			producer->status, producer->id, shared->upper_index, shared->lower_index, lower_case);
			shared->lower_buffer[shared->lower_index++] = lower_case;
			up(sem_proc, lower_full);//sem_post(&lower_full);
			up(sem_proc, all_full);//sem_post(&all_full);

			break;

		default:
			break;
		}
		randomSleep();
	}
}

void* Consumer(void* args){
	role* consumer = (role*) args;
	while(1)
	{
		switch(consumer->type)
		{
		case 1:
			down(sem_proc, upper_full);  //sem_wait(&upper_full);
			down(sem_proc, all_full);//sem_wait(&all_full);
			printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
			consumer->status, consumer->id, shared->upper_index, shared->lower_index, upper_case);
			--shared->upper_index;
			up(sem_proc, upper_empty);//sem_post(&upper_empty);
			up(sem_proc, all_empty);//sem_post(&all_empty);

			break;

		case -1:
			down(sem_proc, lower_full);//sem_wait(&lower_full);
			down(sem_proc, all_full);//sem_wait(&all_full);
			printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
			consumer->status, consumer->id, shared->upper_index, shared->lower_index, lower_case);
			--shared->lower_index;
			up(sem_proc, lower_empty);//sem_post(&lower_empty);
			up(sem_proc, all_empty);//sem_post(&all_empty);

			break;

		case 0:
			if(shared->upper_index >= shared->lower_index)
			{
				down(sem_proc, upper_full);//sem_wait(&upper_full);
				down(sem_proc, all_full);//sem_wait(&all_full);
				printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
				consumer->status, consumer->id, shared->upper_index, shared->lower_index, upper_case);
				--shared->upper_index;
				up(sem_proc, upper_empty);//sem_post(&upper_empty);
				up(sem_proc, all_empty);////sem_post(&all_empty);
			}
			else
			{
				down(sem_proc, lower_full);//sem_wait(&lower_full);
				down(sem_proc, all_full);//sem_wait(&all_full);
				printf(GREEN "I am the %s %d\t Now upper:%d lower: %d\tSo I take item %c from buffer\n" NONE,
				consumer->status, consumer->id, shared->upper_index, shared->lower_index, lower_case);
				--shared->lower_index;
				up(sem_proc, lower_empty);//sem_post(&lower_empty);
				up(sem_proc, all_empty);//sem_post(&all_empty);
			}

			break;

		default:
			break;
		}
		randomSleep();
	}
}

int main(int argc, char**argv)
{
    // 初始化信号量
    sem_proc = get_sem_set(7);
    if(sem_proc == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    union semun sem_args;
    unsigned short array[7] = {MAX_SUM,0,MAX_LOWER,0,MAX_UPPER,0,1};
    sem_args.array = array;
    int ret = semctl(sem_proc, 6, SETALL, sem_args);
    if(ret == -1)
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    // 创建共享内存
    int shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
    if(shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    // 将共享内存连接到当前进程的地址空间
    shared = (struct shared_use_st*)shmat(shmid, 0, 0);
    shared->processID = 0;
    shared->upper_index = 0;
    shared->lower_index = 0;

    role roles[5];
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

    int pids[5];
	int childPid;
    int exit_status[5];
    for (int i = 0; i < 5; i++)
    {
        childPid = fork();
        pids[i] = childPid;
        if (childPid == 0 || childPid == -1) break;
    }
    if (childPid == -1)
    {
		perror("fork error");
		exit(EXIT_FAILURE);
    }

    else if (childPid == 0) //每一个子进程都会运行的代码
    {
        sem_proc = get_sem_set(7);
        //printf("I am the child process!\n");
        //down(sem_proc, sem_process);
        //printf("I am the child process!\n");
        down(sem_proc, sem_process);
        switch(shared->processID){
        case 0:

            shared->processID++;
            up(sem_proc, sem_process);
            Producer((void*)&roles[0]);
            break;
        case 1:
            //down(sem_proc, sem_process);
            shared->processID++;
            up(sem_proc, sem_process);
            Producer((void*)&roles[1]);
            break;
        case 2:
            //down(sem_proc, sem_process);
            shared->processID++;
            up(sem_proc, sem_process);
            Consumer((void*)&roles[2]);
            break;
        case 3:
            //down(sem_proc, sem_process);
            shared->processID++;
            up(sem_proc, sem_process);
            Consumer((void*)&roles[3]);
        break;
        case 4:
            //down(sem_proc, sem_process);
            shared->processID++;
            up(sem_proc, sem_process);
            Consumer((void*)&roles[4]);
        break;
        default:
            break;
        }
        //up(sem_proc, sem_process);
    }
    else
    {
        printf("I am the parent process!\n");
        for (int i = 0; i < 5; i++)
        {
            waitpid(pids[i], &exit_status[i], 0);
            //childPid = fork();
            //pids[i] = childPid;
            //if (childPid == 0 || childPid == -1) break;
        }

    }
}
