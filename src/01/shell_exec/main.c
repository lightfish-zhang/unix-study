#include "apue.h"
#include <sys/wait.h>

/*
    exec  <unistd.h>
    execl ("/bin/ls", "ls", "-1", (char *)0);
    execle ("/bin/ls", "ls", "-l", (char *)0, env);
    execlp ("ls", "ls", "-l", (char *)0);
    execv ("/bin/ls", cmd);
    execve ("/bin/ls", cmd, env);
    execvp ("ls", cmd);
 */

int main(void)
{
    char buf[MAXLINE]; /*from apue.h*/
    pid_t pid;
    int status;

    printf("%% "); /* printf prompt (printf requires %% to print %) */
    while(fgets(buf, MAXLINE, stdin) != NULL){ /* when Ctrl-d, fgets return a NULL pointer */
        if(buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0; /* replace newline with null */

        if((pid = fork()) < 0)
            err_sys("fork error");
        else if(0 == pid){
            execlp(buf, buf, (char *)0); /* because execlp end by null rather than \n */
            /* if execlp error, run continue */
            err_ret("couldn't execute: %s", buf);
            exit(127);
        }

        /* parent */
        if((pid = waitpid(pid, &status, 0)) < 0) /* you can use status to know child how exit */
            err_sys("waitpid error");
        printf("%% ");
    }
    exit(0);
}