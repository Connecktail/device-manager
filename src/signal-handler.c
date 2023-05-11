#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <gtk-3.0/gtk/gtk.h>
#include <pthread.h>

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