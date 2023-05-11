#include <pthread.h>
#include <stdio.h>
#include <msq-utils/msqutils.h>
#include "../include/bottle-taken.h"
#include "../include/screen.h"

pthread_cond_t bottle_taken_condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_bottle_taken_condition = PTHREAD_MUTEX_INITIALIZER;

void handle_bottle_taken(){

    pthread_cond_init(&bottle_taken_condition, NULL);
    pthread_mutex_init(&mutex_bottle_taken_condition, NULL);

    while(1){
        pthread_cond_wait(&bottle_taken_condition, &mutex_bottle_taken_condition);
        update_current_order();
    }   

    pthread_exit(NULL);
}