#include "sem_p.h"


static int comm_sem_set(int nums, int flag)
{
    key_t key = ftok(_PATH_NAME_, _PROJ_ID_);
    if (key < 0)
    {
        perror("ftok");
        return -2;
    }

    return semget((key_t)1234, nums, flag);
}

int create_sem_set(int nums)
{
    int flag = IPC_CREAT | IPC_EXCL | 0644;
    return comm_sem_set(nums, flag);
}

int get_sem_set(int nums)
{
    int flag = IPC_CREAT | 0644;
    return comm_sem_set(nums, flag);
}

int init_sem_set(int sem_id, int which, int val)
{
    union semun un;
    un.val = val;

    return semctl(sem_id, which, SETVAL, un);
}

static int pv(int sem_id, int num, int op)
{
    struct sembuf buf;
    buf.sem_num = num;
    buf.sem_op = op;
    buf.sem_flg = 0;

    return semop(sem_id, &buf, 1);
}

int down(int sem_id, int num)
{
    return pv(sem_id, num, -1);
}

int up(int sem_id, int num)
{
    return pv(sem_id, num, 1);
}

int destroy(int sem_id)
{
    return semctl(sem_id, 0, IPC_RMID);
}
