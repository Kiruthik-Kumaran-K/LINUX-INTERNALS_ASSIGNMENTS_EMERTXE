#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// Signal handler to handle segmentation fault
void handler(int signum, siginfo_t *info, void *ucontext)
{
    // Print segmentation fault message
    printf("Segmentation fault ..!!\n");

    // Print the address that caused the fault
    printf("Address %p caused this error\n", info->si_addr);

    // Exit the program
    exit(1);
}

int main()
{
    struct sigaction act;

    // Initialize the structure to zero
    memset(&act, 0, sizeof(act));

    // Register the signal handler
    act.sa_sigaction = handler;

    // Enable additional signal information
    act.sa_flags = SA_SIGINFO;

    // Register handler for SIGSEGV
    sigaction(SIGSEGV, &act, NULL);

    // Wait for 2 seconds
    sleep(2);

    // Create a pointer with an invalid address
    int *ptr = (int *)0xAA;

    // Access invalid memory to cause segmentation fault
    *ptr = 10;

    return 0;
}