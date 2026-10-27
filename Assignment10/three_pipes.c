#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int pipe1[2], pipe2[2];
    int i, j;
    int cmd1_end, cmd2_end;
    
    if( argc == 1)
    {
        printf("Error: No arguments passed\n");
        printf("Usage: ./three_pipes <command1> '|' <command2> '|' <command3>\n");
        return 1;
    }
    
    for(cmd1_end = 1; cmd1_end < argc; cmd1_end++)
    {
        if(strcmp(argv[cmd1_end], "|") == 0)
            break;
    }
    
    for(cmd2_end = cmd1_end + 1; cmd2_end < argc; cmd2_end++)
    {
        if(strcmp(argv[cmd2_end], "|") == 0)
            break;
    }
    
    if(cmd1_end == argc || cmd2_end == argc || cmd1_end == 1 || cmd2_end ==cmd1_end + 1 || cmd2_end == argc - 1)
    {
        printf("Error: Insufficient arguments passed\n");
        printf("Usage: ./three_pipes <command1> '|' <command2> '|' <command3>\n");
        return 1;
    }
    
    argv[cmd1_end] = NULL;
    argv[cmd2_end] = NULL;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        perror("pipe");
        return 1;
    }
    
    if(fork() == 0)
    {
        dup2(pipe1[1], STDOUT_FILENO);
        
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(1);
    }
    
    if(fork() == 0)
    {
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        execvp(argv[cmd1_end + 1], &argv[cmd1_end + 1]);
        perror("execvp");
        exit(1);
    }
    
    if(fork() == 0)
    {
        dup2(pipe2[0], STDIN_FILENO);
        
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        execvp(argv[cmd2_end + 1], &argv[cmd2_end + 1]);
        perror("execvp");
        exit(1);
    }
    
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    
    for(i = 0 ; i < 3; i++)
        wait(NULL);

    return 0;
}
