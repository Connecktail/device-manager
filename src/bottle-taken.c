#include <pthread.h>
#include <stdio.h>
#include "../include/bottle-taken.h"

pthread_cond_t bottle_taken_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_bottle_taken_condition = PTHREAD_MUTEX_INITIALIZER;

void handle_bottle_taken(){

    pthread_cond_init(&bottle_taken_condition, NULL);
    pthread_mutex_init(&mutex_bottle_taken_condition, NULL);

    while(1){
        printf("waiting for signal\n");
        pthread_cond_wait(&bottle_taken_condition, &mutex_bottle_taken_condition);
        printf("Bottle taken\n");
    }   
}