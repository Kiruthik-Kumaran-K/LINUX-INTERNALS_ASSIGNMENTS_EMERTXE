#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 1234
#define SIZE 1024

struct shared_data
{
    char str[SIZE];
    volatile int ready;
    volatile int done;
};

int main()
{
    int shmid;
    int i, j;
    char temp;

    struct shared_data *shm;

    /* Access existing shared memory */
    shmid = shmget(SHM_KEY, sizeof(struct shared_data), 0666);

    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    /* Attach shared memory */
    shm = (struct shared_data *)shmat(shmid, NULL, 0);

    if (shm == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    /* Wait until Process 1 writes the string */
    while (shm->ready == 0)
    {
        usleep(1000);
    }

    printf("Reading from shared memory: %s\n", shm->str);

    /* Reverse the string */
    i = 0;
    j = strlen(shm->str) - 1;

    while (i < j)
    {
        temp = shm->str[i];
        shm->str[i] = shm->str[j];
        shm->str[j] = temp;

        i++;
        j--;
    }

    /* Write reversed string back to shared memory */
    printf("Writing to shared memory ...\n");

    shm->done = 1;

    /* Detach shared memory */
    shmdt(shm);

    return 0;
}