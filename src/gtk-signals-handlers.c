#include <gtk-3.0/gtk/gtk.h>
#include <semaphore.h>
#include <pthread.h>
#include <msq-utils/message.h>
#include <shm-utils/shmutils.h>

#include "../include/gtk-signals-handlers.h"
#include "../include/screen.h"

extern pthread_cond_t scanner_condition;
extern float price;
extern char barcode[MAX_LENGTH_BARCODE + 2];
extern sem_t send_barcode_semaphore;
extern GtkWidget *pScanBottleModal;
extern GtkWidget *pPairModuleModal;
extern GtkWidget *pPairModuleModalLabel;

void show_scan_bottle_modal()
{
    gtk_widget_show(pScanBottleModal);
}

void hide_scan_bottle_modal()
{
    gtk_widget_hide(pScanBottleModal);
}

void validate_barcode(GtkButton *button, gpointer user_data)
{
    GtkEntry *entry = GTK_ENTRY(user_data);
    const char *data = gtk_entry_get_text(entry);
    strcpy(barcode, data);
    hide_scan_bottle_modal();
    sem_wait(&send_barcode_semaphore);
    if (strcmp(barcode, "") != 0)
        pthread_cond_signal(&scanner_condition);
}

void get_bottle_price(GtkButton *button, gpointer user_data)
{
    GtkEntry *entry = GTK_ENTRY(user_data);
    const char *price_as_text = gtk_entry_get_text(entry);
    if (strcmp(price_as_text, "") == 0)
        price_as_text = "0";
    price = atof(price_as_text);
    sem_post(&send_barcode_semaphore);
}

void show_pair_module_modal()
{
    gtk_label_set_text(GTK_LABEL(pPairModuleModalLabel), "Searching...");
    gtk_widget_show(pPairModuleModal);
    shm_t *shm = get_shm();
    if (shm->main_pid != 0)
        kill(shm->main_pid, SIGUSR1); // Send signal to main process to start pairing
    alarm(PAIRING_WAITING_TIME);      // Set alarm to stop pairing after PAIRING_WAITING_TIME seconds
}

void hide_pair_module_modal()
{
    gtk_widget_hide(pPairModuleModal);
}

void validate_pairing(GtkButton *button, gpointer user_data)
{
    hide_pair_module_modal();
}