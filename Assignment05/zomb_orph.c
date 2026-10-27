#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int print_status(pid_t pid)
{
    char path[50];
    char line[256];
    FILE *fp;

    sprintf(path, "/proc/%d/status", pid);

    fp = fopen(path, "r");

    if (fp == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strncmp(line, "Name:", 5) == 0 || strncmp(line, "State:", 6) == 0)
        {
            printf("%s", line);
        }
    }

    fclose(fp);
    return 1;
}

int main()
{
    pid_t pid, monitor;
    char path[50];
    FILE *fp;

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        sleep(5);
        exit(0);
    }

    printf("A child created with pid %d\n", pid);
    fflush(stdout);

    sleep(1);

    print_status(pid);

    sleep(6);

    print_status(pid);

    monitor = fork();

    if (monitor < 0)
    {
        perror("fork");
        return 1;
    }

    if (monitor == 0)
    {
        while (1)
        {
            sprintf(path, "/proc/%d/status", pid);
            fp = fopen(path, "r");

            if (fp == NULL)
            {
                printf("Process %d cleared by init\n", pid);
                fflush(stdout);
                exit(0);
            }

            fclose(fp);
            sleep(1);
        }
    }

    printf("Parent terminating\n");
    fflush(stdout);

    return 0;
}
