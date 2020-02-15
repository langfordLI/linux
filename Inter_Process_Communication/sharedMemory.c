#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char msg[] = "hello, shared memory\n";
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
    key_t key = ftok("/ipc/sem", 'a');
    int shmid = shmget(key, 1024, IPC_CREAT | 0604); // get shared memory, size for 1M

    int semid = CreateSem(key, 0); // create semaphore
    pid_t p = fork();
    if(p > 0)   // parent process
    {
        printf("parent process id: %d\n", getpid());
        char* shms = (char*)shmat(shmid, 0, 0); // connect shared memory
        memcpy(shms, msg, strlen(msg) + 1);
        sleep(10); // wait for 10s for another process read

        Sem_P(semid);
        shmdt(shms); // disconnect shared memory

        DestroySem(semid); // destory semaphore
    }
    else if(p == 0) // child process
    {
        printf("child process id: %d\n", getpid());
        char* shmc = (char*)shmat(shmid, 0, 0);
        Sem_V(semid);
        printf("shared memory value: %s\n", shmc);
        shmdt(shmc);
    }
    return 0;
}