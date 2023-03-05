#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msq-utils/msqutils.h>
#include <msq-utils/msqtypes.h>

#include "../include/scanner.h"

extern pthread_cond_t scanner_condition;
extern pthread_mutex_t mutex_condition;

extern float price;
int msqid;

void *handle_scanner(void *arg)
{
    printf("Scanner thread started\n");
    msq_msg_t msg;
    msg.mtype = ADD_BOTTLE_TYPE;
    char *barcode = (char *)malloc((MAX_LENGTH_BARCODE + 2) * sizeof(char)); // +2 for \n and \0

    init_message_queue();
    printf("message queue initialized\n");

    while (1)
    {
        pthread_cond_wait(&scanner_condition, &mutex_condition);
        fgets(barcode, MAX_LENGTH_BARCODE + 2, stdin);
        barcode[strlen(barcode) - 1] = '\0';
        strcpy(msg.message.scanner.barcode, barcode);
        msg.message.scanner.price = price;

        printf("barcode scanned : %s, price : %f\n", barcode, price);
        send_message(msg);
    }

    pthread_exit(NULL);
}