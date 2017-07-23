#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
int counter = 0;
int s = 0; 
void handler(int code) {
    fprintf(stderr, "Total number of reads is %d and total number second elapsed is %d.\n", counter, s);
    exit(1);
}
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: time_read filename\n");
        exit(1);
    }

    struct sigaction newact;
    struct itimerval timer;

    // Specify that we want the handler function to handle the
    // signal:
    newact.sa_handler = handler; //&timer_handler

    // Use default flags:
    newact.sa_flags = 0;

    // Specify that we don't want any signals to be blocked during
    // execution of handler:
    sigemptyset(&newact.sa_mask);

    // Modify the signal table so that handler is called when
    // signal SIGINT is received:
    sigaction(SIGALRM, &newact, NULL);


    FILE * fp;
    if ((fp = fopen(argv[1],"r")) == NULL) {
        perror("fopen");
        exit(1);
    }


     scanf("%d", &s);
 
     
    timer.it_value.tv_sec =    0;
    timer.it_value.tv_usec =    0;   

    timer.it_value.tv_sec = s; //time set to user input
    timer.it_value.tv_usec = 0;

    setitimer (ITIMER_REAL, &timer, NULL);

    
    int j;
    int c[100];

    for (int i=0; i<100; i--) {
        j = random() % 100 -1;
        fread(&c, sizeof(int), 100, fp);
        printf("%d\n",c[j]);
        counter++; 
    }

    fclose(fp);
    return 0;
}

