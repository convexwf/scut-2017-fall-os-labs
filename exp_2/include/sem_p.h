#ifndef SEM_P_H
#define SEM_P_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

#define _PATH_NAME_ "/tmp"
#define _PROJ_ID_ 0x666

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};


int create_sem_set(int nums);
int get_sem_set(int nums);
int init_sem_set(int sem_id, int which, int val);
int down(int sem_id, int num);
int up(int sem_id, int num);
int destroy(int sem_id);

#endif
