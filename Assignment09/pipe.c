#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
      int fd[2];
      int pipe_pos;
      int i;

      if(argc == 1)
      {
            printf("Error: No arguments passed\n");
            printf("Usage: ./pipe <command1> '|' <command2>\n");
            return 1;
      }

      for(pipe_pos = 1; pipe_pos < argc; pipe_pos++)
      {
            if(strcmp(argv[pipe_pos], "|") == 0)
                  break;
      }

      if(pipe_pos == argc || pipe_pos == 1 || pipe_pos == argc - 1)
      {
            printf("Error: Insufficient arguments passed\n");
            printf("Usage: ./pipe <command1> '|' <command2>\n");
            return 1;
      }

      argv[pipe_pos] = NULL;

      if(pipe(fd) == -1)
      {
            perror("pipe");
            return 1;
      }

      if(fork() == 0)
      {
            dup2(fd[1], STDOUT_FILENO);

            close(fd[0]);
            close(fd[1]);

            execvp(argv[1], &argv[1]);

            perror("execvp");
            exit(1);
      }

      if(fork() == 0)
      {
            dup2(fd[0], STDIN_FILENO);

            close(fd[0]);
            close(fd[1]);

            execvp(argv[pipe_pos + 1], &argv[pipe_pos + 1]);

            perror("execvp");
            exit(1);
      }

      close(fd[0]);
      close(fd[1]);

      for(i = 0; i < 2; i++)
            wait(NULL);

      return 0;      
}