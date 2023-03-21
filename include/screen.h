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
#include "glade-utils.h"

#define db_host "127.0.0.1"
#define db_database "connecktail"
#define db_user "admin"
#define db_password "admin"

/**
 * @brief thread function to display the screen app and listen to screen interaction
 */
void *display_screen(void *arg);

#endif