#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include "sem_p.h"
#include "color.h"


void randomSleep(){
    int rd = rand()%100 + 1;
    usleep(rd * 10000);
    //sleep(1);
}

struct shared_use_st  {
    int Rid;
    int Wid;
    int content;
    int R_count;
    int W_count;
};

enum sem_id{
    Fmutex = 0,  //写者和读者互斥
    RCOUNT_mutex = 1,
    WCOUNT_mutex = 2,
    CONTENT_mutex = 3,
    id_mutex = 4
};

struct shared_use_st* shared;
int sem_proc;

void Reader(int id)
{

    down(sem_proc, Fmutex);
    down(sem_proc, RCOUNT_mutex);
    if(!shared->R_count) down(sem_proc, CONTENT_mutex);
    shared->R_count++;
    up(sem_proc, RCOUNT_mutex);
    up(sem_proc, Fmutex);
        printf(GREEN "I am the reader %d\tCurrent Content:%d\n" NONE,id,shared->content);
        randomSleep();
    down(sem_proc, RCOUNT_mutex);
    shared->R_count--;
    if(!shared->R_count) up(sem_proc, CONTENT_mutex);
    up(sem_proc, RCOUNT_mutex);
}

void Writer(int id)
{
    randomSleep();
    srand((unsigned)time(NULL));
    int change = rand()%201 - 100;
    down(sem_proc, WCOUNT_mutex);
    if(!shared->W_count) down(sem_proc, Fmutex);
    shared->W_count++;
    up(sem_proc, WCOUNT_mutex);
    down(sem_proc, CONTENT_mutex);
    up(sem_proc, Fmutex);
        printf(RED "I am the writer %d\tCurrent Content:%d\tI am going to change %d\n" NONE,id,shared->content, change);
        shared->content += change;
        randomSleep();
    up(sem_proc, CONTENT_mutex);
    down(sem_proc, WCOUNT_mutex);
    shared->W_count--;
    if(!shared->W_count--) up(sem_proc, RCOUNT_mutex);
    up(sem_proc, WCOUNT_mutex);
}

int main(int argc, char** argv)
{
    // 初始化信号量
    sem_proc = get_sem_set(7);
    if(sem_proc == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    union semun sem_args;
    unsigned short array[5] = {1,1,1,1,1};
    sem_args.array = array;
    int ret = semctl(sem_proc, 4, SETALL, sem_args);
    if(ret == -1)
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
    // 创建共享内存
    int shmid = shmget((key_t)2234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
    if(shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    // 将共享内存连接到当前进程的地址空间
    shared = (struct shared_use_st*)shmat(shmid, 0, 0);
    shared->Rid = 0;
    shared->Wid = 0;
    shared->content = 0;
    shared->R_count = 0;
    shared->W_count = 0;

    int childPid;
    while(1)
    {
        usleep(500000);
        childPid = fork();
        if (childPid == 0 || childPid == -1) break;
    }
    if(childPid == 0)
    {
        srand((unsigned)time(NULL));
        int judge = rand()%2;
        //printf("%d\n", judge);
        if(judge)
        {
            down(sem_proc, id_mutex);
            int ttt = shared->Wid;
            shared->Wid++;
            up(sem_proc, id_mutex);
            Writer(ttt);
        }
        else
        {//printf("I am the hell\n");
            down(sem_proc, id_mutex);
            int ttt = shared->Rid;
            shared->Rid++;
            up(sem_proc, id_mutex);
            Reader(ttt);
        }
    }

}
