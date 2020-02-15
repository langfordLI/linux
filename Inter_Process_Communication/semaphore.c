#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

typedef int sem_t;
union semun
{
    int val; 
    struct semid_ds *buf; // struct point
    unsigned short *array; // array type
}arg;

sem_t CreateSem(key_t key, int value)
{
    union semun sem;
    sem_t semid; // semaphore id
    sem.val = value;

    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0x0666); // get semaphore id
    if(semid == -1)
    {
        printf("create semaphore error\n");
        return -1;
    }
    semctl(semid, 0, SETVAL, sem); // create 0 semaphore
    return semid;
}

int Sem_P(sem_t semid)
{
    struct sembuf sops = {0, +1, IPC_NOWAIT};
    return (semop(semid, &sops, 1));
}

int Sem_V(sem_t semid)
{
    struct sembuf sops = {0, -1, IPC_NOWAIT};
    return (semop(semid, &sops, 1));
}

void SetvalueSem(sem_t semid, int value)
{
    union semun sem;
    sem.val = value;
    semctl(semid, 0, SETVAL, sem);
}
int GetvalueSem(sem_t semid)
{
    union semun sem;
    return semctl(semid, 0, GETVAL, sem);
}

void DestroySem(sem_t semid)
{
    union semun sem;
    sem.val = 0;
    semctl(semid, 0, IPC_RMID, sem);
}

int main()
{
    key_t key;
    key = ftok("/ipc/sem/", 'a');
    
    int semid = CreateSem(key, 100);
    for(int i = 0; i < 4; i++)
    {
        Sem_P(semid);
        Sem_V(semid);
    }
    int value = GetvalueSem(semid);
    printf("semaphore value: %d\n", value);
    DestroySem(semid);

    return 0;
}