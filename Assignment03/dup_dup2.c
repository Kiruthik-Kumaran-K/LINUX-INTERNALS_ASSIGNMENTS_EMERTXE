#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    int saved_stdout;
    int new_fd;

    if (argc != 2)
    {
        printf("Usage: ./dup_dup2 filename\n");
        return 1;
    }

    fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    saved_stdout = dup(STDOUT_FILENO);

    if (saved_stdout == -1)
    {
        perror("dup");
        close(fd);
        return 1;
    }

    close(STDOUT_FILENO);

    new_fd = dup(fd);

    if (new_fd == -1)
    {
        perror("dup");
        dup2(saved_stdout, STDOUT_FILENO);
        close(fd);
        close(saved_stdout);
        return 1;
    }

    printf("This message is written to the file using dup()\n");
    fflush(stdout);

    dup2(saved_stdout, STDOUT_FILENO);

    printf("Output redirected back to STDOUT\n");
    fflush(stdout);

    close(saved_stdout);
    close(fd);

    return 0;
}
