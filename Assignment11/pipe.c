#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int cmd_count = 1;
    int i, j, k;
    int start = 1;
    int pipe_fd[2];
    int prev_read = -1;
    pid_t pid;
    int status;

    /* Check whether arguments are passed */
    if (argc < 2)
    {
        printf("Error: No arguments passed, provide at least one pipe or more\n");
        printf("Usage: ./pipe <command1> '|' <command2>\n");
        printf("Usage: ./pipe <command1> '|' <command2> '|' <command3> etc\n");
        return 1;
    }

    /* Count the number of commands */
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            cmd_count++;
        }
    }

    /* Validate pipe placement */
    if (strcmp(argv[1], "|") == 0 || strcmp(argv[argc - 1], "|") == 0)
    {
        printf("Error: Invalid pipe placement\n");
        return 1;
    }

    for (i = 1; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "|") == 0 && strcmp(argv[i + 1], "|") == 0)
        {
            printf("Error: Empty command between pipes\n");
            return 1;
        }
    }


    for (k = 0; k < cmd_count; k++)
    {
        /* Find the end of the current command */
        j = start;

        while (j < argc && strcmp(argv[j], "|") != 0)
        {
            j++;
        }

        /* Save the next command's starting position */
        int next_start = j + 1;

        /* Terminate the current command's arguments */
        if (j < argc)
        {
            argv[j] = NULL;
        }

        /* Create a pipe unless this is the last command */
        if (k < cmd_count - 1)
        {
            if (pipe(pipe_fd) == -1)
            {
                perror("pipe");
                return 1;
            }
        }

        /* Create a child process */
        pid = fork();

        if (pid == -1)
        {
            perror("fork");
            return 1;
        }

        if (pid == 0)
        {
            /* Read input from the previous command */
            if (prev_read != -1)
            {
                if (dup2(prev_read, STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
                close(prev_read);
            }

            /* Send output to the next command */
            if (k < cmd_count - 1)
            {
                close(pipe_fd[0]);

                if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }

                close(pipe_fd[1]);
            }

            /* Execute the command */
            execvp(argv[start], &argv[start]);

            perror(argv[start]);
            exit(127);
        }

        /* Parent closes the previous pipe's read end */
        if (prev_read != -1)
        {
            close(prev_read);
        }

        /* Keep the current pipe's read end for the next child */
        if (k < cmd_count - 1)
        {
            close(pipe_fd[1]);
            prev_read = pipe_fd[0];
        }

        /* Move to the next command */
        start = next_start;
    }

    /* Wait for all child processes */
    for (i = 0; i < cmd_count; i++)
    {
        wait(&status);
    }

    return 0;
}
