#ifndef SCREEN_H
#define SCREEN_H

#include <gtk-3.0/gtk/gtk.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <db-utils/dbutils.h>
#include <shm-utils/shmutils.h>
#include "glade-utils.h"

#define db_host "127.0.0.1"
#define db_database "connecktail"
#define db_user "admin"
#define db_password "admin"

#define PAIRING_WAITING_TIME 30

typedef struct {
    id_db_t id_order;
    int percentage;
    int step;
    int total_step;
    int bottle;
    int total_bottle;
    char *message;
} status_t;

typedef struct {
   order_t *order;
   int step;
   int total_step;
   int bottle;
   int total_bottle;
   step_t **current_cocktail_steps;
} current_order_t;


/**
 * @brief thread function to display the screen app and listen to screen interaction
 */
void *display_screen(void *arg);

/**
 * @brief signal handler to update the screen
 */
void signal_handler();

/**
 * @brief update the screen
 * @note this function is called by the signal handler
 */
gboolean update_screen();

/**
 * @brief close the app
 */
void close_app();

/**
 * @brief go to the administration page
 * @note handler for the button "Administration"
 */
void go_to_admin();

/**
 * @brief go to the homepage
 * @note handler for the button "Homepage"
 */
void go_to_homepage();

#endif