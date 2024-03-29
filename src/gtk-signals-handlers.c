#include <gtk-3.0/gtk/gtk.h>
#include <semaphore.h>
#include <pthread.h>
#include <msq-utils/message.h>
#include <shm-utils/shmutils.h>

#include "../include/gtk-signals-handlers.h"
#include "../include/screen.h"
#include <locale.h>

extern PGconn *conn;
extern cocktail_t *cocktail_added;
extern bottle_t **bottles;
extern module_t **modules;
extern int length;
extern int msqid;

extern GtkBuilder *builder;
extern GtkWidget *stack, *add_cocktail_stack;
extern GtkWidget *pHomepage, *pAdministration, *pAddCocktail, *pPairModuleBox, *pPairModuleModalLabel, *pValidatePairingButton, *pAssociateModuleBottleBox;
extern GtkWidget *pScanBottleModal, *pAddCocktailModal, *pScanBottleModal;
extern GtkWidget *pCocktailInfos, *pBottlesSelection, *pStepInfos;
extern GtkBox *bottles_selection_list, *modules_list;

extern pthread_cond_t scanner_condition;
extern float price;
extern char barcode[MAX_LENGTH_BARCODE + 2];
extern sem_t send_barcode_semaphore;

extern current_order_t *current_order;
extern int add_cocktail_step, nb_step, nb_bottles;
extern step_data_t **bottle_data_list;

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
    gtk_stack_set_visible_child(GTK_STACK(add_cocktail_stack), pCocktailInfos);
    add_cocktail_step = 1;
    GtkEntry *pCocktailName = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_name"));
    GtkEntry *pCocktailPrice = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_price"));
    GtkEntry *pCocktailDesc = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_description"));
    gtk_entry_set_text(pCocktailName, "");
    gtk_entry_set_text(pCocktailPrice, "");
    gtk_entry_set_text(pCocktailDesc, "");
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
    switch (add_cocktail_step)
    {
    case 1:
        gtk_stack_set_visible_child(GTK_STACK(stack), pAdministration);
        break;
    case 2:
        gtk_stack_set_visible_child(GTK_STACK(add_cocktail_stack), pCocktailInfos);
        add_cocktail_step--;
        break;
    case 3:
        gtk_stack_set_visible_child(GTK_STACK(add_cocktail_stack), pBottlesSelection);
        add_cocktail_step--;
        break;
    default:
        add_cocktail_step--;
        save_step_info(bottle_data_list[add_cocktail_step - 2]);
        update_step_info(bottle_data_list[add_cocktail_step - 3]);
        break;
    }
}

void next_add_cocktail()
{
    switch (add_cocktail_step)
    {
    case 1:
        nb_step = 0;
        cocktail_added = get_cocktail_info();
        cocktail_added->personalized = false;

        GList *children, *iter;
        children = gtk_container_get_children(GTK_CONTAINER(bottles_selection_list));
        for (iter = children; iter != NULL; iter = g_list_next(iter))
        {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        g_list_free(children);

        bottles = get_bottles(conn, &nb_bottles);
        bottle_data_list = (step_data_t **)malloc(sizeof(step_data_t *) * (nb_bottles));
        for (int i = 0; i < nb_bottles; i++)
        {
            bottle_data_list[i] = (step_data_t *)malloc(sizeof(step_data_t));
            bottle_data_list[i]->bottle = bottles[i];
            bottle_data_list[i]->bottle_item = GTK_BOX(make_bottle_item_addcocktail(bottle_data_list[i]));
            bottle_data_list[i]->checked = 0;
            bottle_data_list[i]->position = i;

            gtk_box_pack_start(bottles_selection_list, GTK_WIDGET(bottle_data_list[i]->bottle_item), TRUE, TRUE, 0);
        }
        gtk_widget_show_all(GTK_WIDGET(bottles_selection_list));

        gtk_stack_set_visible_child(GTK_STACK(add_cocktail_stack), pBottlesSelection);
        add_cocktail_step++;
        break;
    case 2:
        if (nb_step != 0)
        {
            gtk_stack_set_visible_child(GTK_STACK(add_cocktail_stack), pStepInfos);
            update_step_info(bottle_data_list[0]);
            add_cocktail_step++;
        }
        break;
    default:
        if (add_cocktail_step < nb_step + 2)
        {
            save_step_info(bottle_data_list[add_cocktail_step - 3]);
            update_step_info(bottle_data_list[add_cocktail_step - 2]);
            add_cocktail_step++;
        }
        else
        {
            save_step_info(bottle_data_list[add_cocktail_step - 3]);

            strcpy(cocktail_added->image_url, "https://image.jpg");
            setlocale(LC_ALL, "en_US.UTF-8");
            insert_cocktail(conn, cocktail_added);

            for (int i = 0; i < nb_step; i++)
            {
                id_db_t id = cocktail_added->id;
                bottle_data_list[i]->step->id_cocktail = (id_db_t)malloc(sizeof(id_db_t));
                *bottle_data_list[i]->step->id_cocktail = *id;
                strcpy(bottle_data_list[i]->step->message, "");
                insert_step(conn, bottle_data_list[i]->step);
            }

            free(bottle_data_list);

            update_cocktail_list();
            go_to_admin();
        }
        break;
    }
}

void check_bottle_clicked(GtkButton *button)
{
    GtkBox *parent = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(button)));
    GtkBox *bottle_item = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(parent)));

    step_data_t *st = g_object_get_data(G_OBJECT(bottle_item), "step_data");

    if (st->checked == 1)
    {
        GList *children, *iter;
        children = gtk_container_get_children(GTK_CONTAINER(parent));
        for (iter = children; iter != NULL; iter = g_list_next(iter))
        {
            if (strcmp(gtk_widget_get_name(GTK_WIDGET(iter->data)), "buttons_box") == 0)
            {
                gtk_widget_destroy(GTK_WIDGET(iter->data));
            }
        }
        g_list_free(children);

        gtk_box_reorder_child(bottles_selection_list, GTK_WIDGET(bottle_item), nb_step - 1);

        step_data_t *current = bottle_data_list[st->position];

        for (int i = st->position; i < nb_step - 1; i++)
        {
            bottle_data_list[i] = bottle_data_list[i + 1];
            bottle_data_list[i]->position = i;
        }

        bottle_data_list[nb_step - 1] = current;

        st->checked = 0;
        st->position = nb_step - 1;

        free(bottle_data_list[nb_step]->step);
        nb_step--;
    }
    else
    {
        GtkBox *buttons_box = GTK_BOX(make_buttons_box());

        gtk_box_pack_start(parent, GTK_WIDGET(buttons_box), FALSE, FALSE, 0);

        gtk_box_reorder_child(parent, GTK_WIDGET(buttons_box), 0);
        gtk_box_reorder_child(bottles_selection_list, GTK_WIDGET(bottle_item), nb_step);

        step_data_t *current = bottle_data_list[st->position];

        for (int i = st->position; i > nb_step; i--)
        {
            bottle_data_list[i] = bottle_data_list[i - 1];
            bottle_data_list[i]->position = i;
        }

        bottle_data_list[nb_step] = current;

        bottle_data_list[nb_step]->checked = 1;
        bottle_data_list[nb_step]->position = nb_step;
        bottle_data_list[nb_step]->step_completed = 0;
        nb_step++;
    }

    gtk_widget_show_all(GTK_WIDGET(parent));
}

void control_button_bottle_clicked(GtkButton *button, gpointer b_data)
{
    unsigned data = (unsigned)(uintptr_t)b_data;

    GtkBox *parent = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(gtk_widget_get_parent(GTK_WIDGET(button)))));
    GtkBox *bottle_item = GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(parent)));

    step_data_t *st = g_object_get_data(G_OBJECT(bottle_item), "step_data");

    if ((data == 1 && st->position > 0) || (data == -1 && st->position < nb_step - 1))
    {
        gtk_box_reorder_child(bottles_selection_list, GTK_WIDGET(bottle_item), st->position - data);

        step_data_t *current = bottle_data_list[st->position];

        bottle_data_list[st->position] = bottle_data_list[st->position - data];
        bottle_data_list[st->position - data] = current;

        bottle_data_list[st->position]->position = st->position;
        bottle_data_list[st->position - data]->position = st->position - data;
    }
}

void start_order_clicked(GtkButton *button)
{
    if (current_order == NULL)
    {
        order_t *order = g_object_get_data(G_OBJECT(button), "order");

        int new_status = 1;
        update_order(conn, order, NULL, &new_status);

        update_orders_list();
        init_current_order(order);
    }
}


void show_pair_module_modal()
{
    gtk_stack_set_visible_child(GTK_STACK(stack), pPairModuleBox);
    gtk_label_set_text(GTK_LABEL(pPairModuleModalLabel), "Searching...");
    shm_t *shm = get_shm();
    if (shm->main_pid != 0)
        kill(shm->main_pid, SIGUSR1); // Send signal to main process to start pairing
    alarm(PAIRING_WAITING_TIME);      // Set alarm to stop pairing after PAIRING_WAITING_TIME seconds
}

void hide_pair_module_modal()
{
    gtk_stack_set_visible_child(GTK_STACK(stack), pAdministration);
}

void validate_pairing()
{
    hide_pair_module_modal();
}

void show_associate_module_bottle()
{
    // Remove all modules from list
    GList *children = gtk_container_get_children(GTK_CONTAINER(modules_list));
    GList *iter;
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_container_remove(GTK_CONTAINER(modules_list), GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Add modules to list
    gtk_stack_set_visible_child(GTK_STACK(stack), pAssociateModuleBottleBox);
    modules = get_modules(conn, &length);
    for (int i = 0; i < length; i++)
    {
        gtk_box_pack_start(modules_list, GTK_WIDGET(make_module_item(modules[i])), TRUE, TRUE, 0);
    }
}

void back_associate_module_bottle()
{
    gtk_stack_set_visible_child(GTK_STACK(stack), pAdministration);
}

void associate_module_clicked(GtkButton *button, gpointer b_data)
{
    toggle_button_data_t *data = (toggle_button_data_t *)b_data;
    GtkBox *non_associated_bottles_box = data->non_associated_bottles;
    GtkButton *toggle_button = data->toggle_button;
    
    if (gtk_widget_get_visible(GTK_WIDGET(non_associated_bottles_box))){
        gtk_widget_set_visible(GTK_WIDGET(non_associated_bottles_box), FALSE);
        gtk_button_set_label(toggle_button, "Show bottles");
    }else{
        gtk_widget_set_visible(GTK_WIDGET(non_associated_bottles_box), TRUE);
        gtk_button_set_label(toggle_button, "Hide bottles");
    }
}

void dissociate_module_clicked(GtkButton *button, gpointer b_data)
{
    module_t *module = (module_t *)malloc(sizeof(module_t));
    
    strcpy(module->mac_address, (char *)b_data);
    dissociate_module(conn, module);
    back_associate_module_bottle();
}

void light_led()
{
    ip_address_t *ip_address = get_ip_adresse_of_module_from_current_order();
    if (ip_address != NULL)
    {
        // send message led_message_t
        msq_msg_t msg;
        msg.mtype = ENABLE_LED_TYPE;
        strcpy(msg.message.led.ip_address, ip_address);
        send_message(msg);
    }else{
        g_printerr("No module associated to current order\n");
    }
}

void choose_bottle_to_associate(GtkButton *button, gpointer b_data){
    association_data_t *data = (association_data_t*)b_data;
    module_t *module = data->module;
    bottle_t *bottle = data->bottle;

    associate_bottle(conn, bottle, module);
    back_associate_module_bottle();
}

void play_sound()
{
    ip_address_t *ip_address = get_ip_adresse_of_module_from_current_order();
    if (ip_address != NULL)
    {
        msq_msg_t msg;
        msg.mtype = ENABLE_BUZZER_TYPE;
        strcpy(msg.message.buzzer.ip_address, ip_address);
        send_message(msg);
    }
}