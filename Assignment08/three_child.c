#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
      pid_t pid[3];
      int status;
      int i;
      pid_t child_pid;

      for(i = 0; i < 3; i++)
      {
            pid[i] = fork();

            if(pid[i] == 0)
            {
                  exit(0);
            }
            else if(pid[i] > 0)
            {
                  printf("Child %d with pid %d created\n", i + 1, pid[i]);
            }
            else
            {
                  perror("fork");
                  return 1;
            }
      }

      for(i = 0; i < 3; i++)
      {
            child_pid = wait(&status);

            if(child_pid > 0)
            {
                  if(WIFEXITED(status))
                  {
                        printf("Child %d is terminated with code %d\n", child_pid, WEXITSTATUS(status));
                  }
            }
      }
      return 0;
}