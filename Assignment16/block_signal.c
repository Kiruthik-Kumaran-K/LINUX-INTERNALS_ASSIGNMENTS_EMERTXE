#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Signal handler for SIGINT
void handler(int signum)
{
    printf("\nSIGINT received\n");

    // Handler runs for some time
    for (int i = 0; i < 10; i++)
    {
        printf("Inside handler\n");
        sleep(1);
    }

    printf("Exiting handler\n");
}

int main(void)
{
    struct sigaction act;
    sigset_t signals;

    // Create an empty signal set
    sigemptyset(&signals);

    // Add signals to be blocked inside the handler
    sigaddset(&signals, SIGUSR1);
    sigaddset(&signals, SIGTERM);
    sigaddset(&signals, SIGABRT);

    // Configure the signal handler
    act.sa_handler = handler;
    act.sa_mask = signals;
    act.sa_flags = 0;

    // Register handler for SIGINT
    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }

    printf("Process %d waiting for signal.. press Ctrl+C\n", getpid());

    // Keep the process running
    while (1)
    {
        pause();
    }

    return 0;
}