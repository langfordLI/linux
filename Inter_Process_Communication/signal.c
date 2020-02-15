#include <signal.h>
#include <stdio.h>
static void sig_handle(int signo)
{
    if(signo == SIGSTOP) // if SIGSTOP signal
    {
        printf("receive SIGSTOP\n");
    }
    else if(signo == SIGKILL)
    {
        printf("receive SIGKILL\n");
    }
    else
    {
        printf("receive signal: %d\n", signo);
    }
    
}

int main()
{
    __sighandler_t ret;
    ret = signal(SIGSTOP, sig_handle); // connect SIGSTOP signal
    if(SIG_ERR == ret)
    {
        printf("failed to connect SIGSTOP\n");
        return -1;
    }
    ret = signal(SIGKILL, sig_handle);
    if(ret == SIG_ERR)
    {
        printf("failed to connect to SIGKILL\n");
        return -1;
    }
    for(;;);
    return 0;
}