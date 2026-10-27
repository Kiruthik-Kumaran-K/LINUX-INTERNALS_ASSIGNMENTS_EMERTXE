#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
      pid_t pid;
      int status;

      if(argc < 2)
      {
            printf("Usage: ./exe_child args ...\n");
            return 1;
      }

      pid = fork();

      if(pid == 0)
      {
            printf("This is the CHILD process, with id %d\n",getpid());
            fflush(stdout);

            execvp(argv[1], &argv[1]);

            perror("execvp");
            exit(1);
      }
      else if(pid > 0)
      {
            wait(&status);

            if(WIFEXITED(status))
            {
                  printf("Child exited with status %d\n",WEXITSTATUS(status));
            }
      }
      else
      {
            perror("fork");
            return 1;
      }

      return 0;

}