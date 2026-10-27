#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    struct shared_data *shm;

    /* Create shared memory */
    shmid = shmget(SHM_KEY, sizeof(struct shared_data), IPC_CREAT | 0666);

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

    /* Initialize synchronization flags */
    shm->ready = 0;
    shm->done = 0;

    /* Read input */
    printf("Enter the string: ");
    fflush(stdout);

    if (fgets(shm->str, SIZE, stdin) == NULL)
    {
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    shm->str[strcspn(shm->str, "\n")] = '\0';

    /* Convert to uppercase */
    for (int i = 0; shm->str[i] != '\0'; i++)
    {
        shm->str[i] = toupper((unsigned char)shm->str[i]);
    }

    printf("Writing to shared memory ......\n");

    /* Tell Process 2 that data is ready */
    shm->ready = 1;

    /* Wait for Process 2 to finish */
    while (shm->done == 0)
    {
        usleep(1000);
    }

    /* Read the reversed string */
    printf("Reading from shared memory: %s\n", shm->str);

    /* Detach and remove shared memory */
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}