#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
      pid_t pid;
      pid_t ret;
      int status;

      pid = fork();

      if(pid == 0)
      {
            sleep(5);
      }
      else if(pid > 0)
      {
            printf("A child created with pid %d\n",pid);

            while(1)
            {
                  ret = waitpid(pid, &status, WNOHANG);
                  if(ret == pid)
                  {
                        if(WIFEXITED(status))
                        {
                              printf("Child %d terminated normally with exit status %d\n",pid, WEXITSTATUS(status));
                        }
                  break;
                  }
                  else if(ret == 0)
                  {
                        printf("parent is running\n");
                        sleep(1);
                  }
                  else
                  {
                        perror("waitpid");
                        return 1;
                  }
            }

            printf("Parent terminating\n");
      }
      else
      {
            perror("fork");
            return 1;
      }

      return 0;
}