#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/scanner.h"

pthread_cond_t scanner_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_condition = PTHREAD_MUTEX_INITIALIZER;

float price;

int main()
{
    pthread_t th_scanner;
    pthread_cond_init(&scanner_condition, NULL);
    pthread_mutex_init(&mutex_condition, NULL);

    pthread_create(&th_scanner, NULL, handle_scanner, NULL);

    sleep(5);

    // Simulation of a click on the button
    printf("Click on the button\n");
    price = 1.5;
    pthread_cond_signal(&scanner_condition);

    pthread_join(th_scanner, NULL);

    return 0;
}