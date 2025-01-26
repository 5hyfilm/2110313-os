#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int compute_period, sleep_period, i;
void on_alarm(int signal)
{
    printf("Sleep\n");
    sleep(sleep_period);
    printf("Wake up\n");

    alarm(compute_period);
}

int main()
{
    printf("Enter compute period :\n");
    scanf("%d", &compute_period);

    printf("Enter sleep period :\n");
    scanf("%d", &sleep_period);

    signal(SIGALRM, on_alarm);
    alarm(compute_period);
    for (i = 0;; i++)
    {
        if (i == 0)
            printf("computing\n");
    }
}
