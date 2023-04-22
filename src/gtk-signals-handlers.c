#include <gtk-3.0/gtk/gtk.h>
#include <semaphore.h>
#include <pthread.h>
#include <msq-utils/message.h>

#include "../include/gtk-signals-handlers.h"

extern GtkBuilder *builder;

extern GtkWidget *stack,  *addCocktailStack;
extern GtkWidget *pHomepage, *pAdministration, *pAddCocktail;
extern pthread_cond_t scanner_condition;
extern float price;
extern char barcode[MAX_LENGTH_BARCODE + 2];
extern sem_t send_barcode_semaphore;
extern GtkBox *bottles_selection_list;
extern GtkWidget *pScanBottleModal, *pAddCocktailModal;
extern GtkWidget *pCocktailInfos, *pBottlesSelection, *pStepInfos;
extern int add_cocktail_step;
extern cocktail_t *cocktail_added;


void go_to_admin()
{
    gtk_stack_set_visible_child(GTK_STACK(stack), pAdministration);
}
void go_to_homepage()
{
    gtk_stack_set_visible_child(GTK_STACK(stack), pHomepage);
}

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
    gtk_stack_set_visible_child(GTK_STACK(stack), pAddCocktail);
    gtk_stack_set_visible_child(GTK_STACK(addCocktailStack), pCocktailInfos);
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

void back_add_cocktail()
{
    switch(add_cocktail_step) {
        case 1:
            gtk_stack_set_visible_child(GTK_STACK(stack), pAdministration);
            break;
        case 2:
            gtk_stack_set_visible_child(GTK_STACK(addCocktailStack), pCocktailInfos);
            add_cocktail_step--;
            break;
        default:
            break;
    }
}

void next_add_cocktail() {
    switch(add_cocktail_step) {
        case 1:
            gtk_stack_set_visible_child(GTK_STACK(addCocktailStack), pBottlesSelection);

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

            add_cocktail_step++;      
            break;
        case 2:
            GtkLabel *order_label = GTK_LABEL(gtk_label_new("jgioerjgjreiogjreio"));

            gtk_box_pack_start(GTK_BOX(bottles_selection_list), GTK_WIDGET(order_label), FALSE, FALSE, 0);  

            break;
        default:
            break;
    }
}