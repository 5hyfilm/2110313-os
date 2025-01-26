#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main()
{
    int i;
    int n = 3;
    pid_t childpid, grandchildpid;

    childpid = fork();
    if (childpid == 0)
    {
        printf("I am the child process. My PID is %ld and my parent's PID is %ld\n",
               (long)getpid(), (long)getppid());
        grandchildpid = fork();

        if (grandchildpid == 0)
        {
            printf("I am the grandchild process. My PID is %ld and my parent's PID is %ld\n",
                   (long)getpid(), (long)getppid());
        }
    }
    else if (childpid > 0)
    {
        printf("I am the parent process. My PID is %ld\n", (long)getpid());
    }
    else
    {
        perror("fork");
        return 1;
    }
    return 0;
}