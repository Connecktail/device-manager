#include <gtk-3.0/gtk/gtk.h>
#include <semaphore.h>
#include <pthread.h>
#include <msq-utils/message.h>

#include "../include/gtk-signals-handlers.h"

extern GtkBuilder *builder;
extern GtkWidget *add_cocktail_stack;
extern pthread_cond_t scanner_condition;
extern float price;
extern char barcode[MAX_LENGTH_BARCODE + 2];
extern sem_t send_barcode_semaphore;
extern GtkWidget *pScanBottleModal, *pAddCocktailModal;
extern GtkWidget *pCocktailInfos, *pBottlesSelection, *pStepInfos;
extern int add_cocktail_step;
extern cocktail_t *cocktail_added;

void show_scan_bottle_modal()
{
    gtk_widget_show(pScanBottleModal);
}

void hide_scan_bottle_modal()
{
    gtk_widget_hide(pScanBottleModal);
}

void show_add_cocktail_modal()
{
    gtk_widget_show(pAddCocktailModal);
    add_cocktail_step = 1;
}

void hide_add_cocktail_modal()
{
    gtk_widget_hide(pAddCocktailModal);
    add_cocktail_step = 0;
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

void continue_add_cocktail_modal() {
    switch(add_cocktail_step) {
        case 1:
            GtkEntry *pCocktailName = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_name"));
            GtkEntry *pCocktailPrice = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_price"));
            GtkEntry *pCocktailDesc = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_description"));
            const char *cocktail_name = gtk_entry_get_text(pCocktailName);
            const char *cocktail_price_txt = gtk_entry_get_text(pCocktailPrice);
            const char *cocktail_desc = gtk_entry_get_text(pCocktailDesc);
            if (strcmp(cocktail_price_txt, "") == 0)
                cocktail_price_txt = "0";

            float cocktail_price = atof(cocktail_price_txt);

            strcpy(cocktail_added->name, cocktail_name);
            strcpy(cocktail_added->description, cocktail_desc);
            cocktail_added->price = cocktail_price;

            gtk_stack_set_visible_child(GTK_STACK(add_cocktail_stack), pBottlesSelection);
            break;
        default:
            break;
    }
}