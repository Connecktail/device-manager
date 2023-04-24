#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <msq-utils/message.h>

#include "../include/scanner.h"
#include "../include/screen.h"
#include "../include/signal-handler.h"

pthread_cond_t scanner_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_condition = PTHREAD_MUTEX_INITIALIZER;

float price;
char barcode[MAX_LENGTH_BARCODE + 2];
pthread_t th_scanner, th_screen;
int shmid;

int main()
{
    struct sigaction newact;
    newact.sa_handler = signal_handler_linux;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGUSR1, &newact, NULL);
    sigaction(SIGALRM, &newact, NULL);

    shmid = init_shared_memory();
    change_device_handler_pid(getpid());

    pthread_cond_init(&scanner_condition, NULL);
    pthread_mutex_init(&mutex_condition, NULL);

    pthread_create(&th_scanner, NULL, handle_scanner, NULL);
    pthread_create(&th_screen, NULL, display_screen, NULL);

    pthread_join(th_screen, NULL);
    pthread_join(th_scanner, NULL);

    return 0;
}