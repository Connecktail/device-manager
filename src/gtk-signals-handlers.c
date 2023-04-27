#include <gtk-3.0/gtk/gtk.h>
#include <semaphore.h>
#include <pthread.h>
#include <msq-utils/message.h>
#include <shm-utils/shmutils.h>

#include "../include/gtk-signals-handlers.h"
#include "../include/screen.h"

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
extern int add_cocktail_step, nb_step;
extern cocktail_t *cocktail_added;
extern GtkWidget *pScanBottleModal;
extern GtkWidget *pPairModuleModal;
extern GtkWidget *pPairModuleModalLabel;

extern step_data_t **bottle_data_list;
extern int nb_bottles;

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

            // for (int i = 0; i < nb_bottles; i++) {
            //     printf("id: %lld, pos:%d, name:>%s<\n", *bottle_data_list[i].bottle->id, bottle_data_list[i].position, bottle_data_list[i].bottle->name);
            //     gtk_box_pack_start(bottles_selection_list, GTK_WIDGET(make_bottle_item_addcocktail(&bottle_data_list[i])), FALSE, FALSE, 0);  
            // }
            add_cocktail_step++;   

            
    for(int i = 0; i < nb_bottles; i++) {
        printf("id: %lld, pos:%d, name:>%s<\n", *bottle_data_list[i]->bottle->id, bottle_data_list[i]->position, bottle_data_list[i]->bottle->name);
    }   
            break;
        case 2:
            
            break;
        default:
            break;
    }
}

void check_bottle_clicked(GtkButton *button) {
    step_data_t *st = g_object_get_data(G_OBJECT(button), "step_data");

    GtkBox *parent = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(button)));

    if(st->checked == 1) {
        GList *children, *iter;
        children = gtk_container_get_children(GTK_CONTAINER(parent));
        for (iter = children; iter != NULL; iter = g_list_next(iter)){
            if(strcmp(gtk_widget_get_name(GTK_WIDGET(iter->data)), "buttons_box") == 0) {
                gtk_widget_destroy(GTK_WIDGET(iter->data));
            }
        }
        g_list_free(children);

        GtkBox *bottle_item = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(parent)));
        
        gtk_box_reorder_child(bottles_selection_list, GTK_WIDGET(bottle_item), nb_step-1);

        step_data_t *current = bottle_data_list[st->position];

        for(int i = st->position; i < nb_step-1; i++) {
            printf("i:%d\n", i);
            bottle_data_list[i] = bottle_data_list[i+1];
            bottle_data_list[i]->position = i;
        }

        bottle_data_list[nb_step-1] = current;

        st->checked = 0;
        st->position = nb_step-1;
        nb_step--;
    } else {
        GtkBox *buttons_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

        GtkButton *up_button = GTK_BUTTON(gtk_button_new_with_label("▲"));
        GtkButton *down_button = GTK_BUTTON(gtk_button_new_with_label("▼"));

        gtk_widget_set_name(GTK_WIDGET(buttons_box), "buttons_box");

        gtk_box_pack_start(buttons_box, GTK_WIDGET(up_button), FALSE, FALSE, 0);
        gtk_box_pack_start(buttons_box, GTK_WIDGET(down_button), FALSE, FALSE, 0);
        gtk_box_pack_start(parent, GTK_WIDGET(buttons_box), FALSE, FALSE, 0); 

        gtk_box_reorder_child(parent, GTK_WIDGET(buttons_box), 0);

        GtkBox *bottle_item = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(parent)));

        gtk_box_reorder_child(bottles_selection_list, GTK_WIDGET(bottle_item), nb_step);


        //reorder bottle_data_list
        step_data_t *current = bottle_data_list[st->position];

        for(int i = st->position; i > nb_step; i--) {
            bottle_data_list[i] = bottle_data_list[i-1];
            bottle_data_list[i]->position = i;
        }

        bottle_data_list[nb_step] = current;

        bottle_data_list[nb_step]->checked = 1;
        bottle_data_list[nb_step]->position = nb_step;
        nb_step++;
    }

    for(int i = 0; i < nb_bottles; i++) {
        printf("id: %lld, pos:%d, che:%d, name:>%s<\n", *bottle_data_list[i]->bottle->id, bottle_data_list[i]->position, bottle_data_list[i]->checked, bottle_data_list[i]->bottle->name);
    }

    gtk_widget_show_all(GTK_WIDGET(parent));
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