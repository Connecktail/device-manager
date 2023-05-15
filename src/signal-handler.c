#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <gtk-3.0/gtk/gtk.h>
#include <pthread.h>
#include "../include/screen.h"

extern pthread_cond_t bottle_taken_condition;
extern GtkWidget *pPairModuleModalLabel;
int module_found = 0;

void signal_handler_linux(int signum)
{
    if (signum == SIGUSR1)
    {
        // The module is paired, modify the label of the pari_module_modal
        gtk_label_set_text(GTK_LABEL(pPairModuleModalLabel), "New module found");
        module_found = 1;
    }
    else if(signum == SIGUSR2)
    {
        // New order has just arrived, update the orders list
        g_idle_add_full(G_PRIORITY_HIGH_IDLE, update_orders_list, NULL, NULL);
    }
    else if (signum == SIGPOLL)
    {
        // New bottle scanned, update the bottles list
        g_idle_add_full(G_PRIORITY_HIGH_IDLE, update_bottles_list, NULL, NULL);
    }
    else if (signum == SIGALRM && !module_found)
    {
        // The module is not paired, modify the label of the pari_module_modal
        gtk_label_set_text(GTK_LABEL(pPairModuleModalLabel), "No module found");
    }
    else if (signum == SIGPROF)
    {
        // A step has been done, send message via the msq and change display
        pthread_cond_signal(&bottle_taken_condition);
    }
}