#include "../include/screen.h"

extern PGconn *conn;
extern GtkBuilder *builder;
extern current_order_t *current_order;
extern int nb_step;

void init_current_order(order_t *order)
{
    GtkBox *current_order_controls = GTK_BOX(gtk_builder_get_object(builder, "current-order-controls"));
    gtk_widget_show_all(GTK_WIDGET(current_order_controls));

    current_order = (current_order_t *)malloc(sizeof(current_order_t));
    current_order->order = get(conn, order->id);
    current_order->cocktail = 0;
    current_order->total_cocktail = current_order->order->nb_cocktails;
    current_order->step = 0;
    current_order->total_step = 0;
    current_order->bottle = 0;
    current_order->total_bottle = 0;

    int nb_steps;

    for (int i = 0; i < current_order->total_cocktail; i++)
    {
        get_cocktail_steps(conn, &nb_steps, current_order->order->cocktails[i]->id);
        current_order->total_step += nb_steps;
    }

    update_current_order();
}

void update_current_order()
{

    GtkBox *current_order_box = GTK_BOX(gtk_builder_get_object(builder, "current-order"));

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(current_order_box));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    if (current_order->bottle == current_order->total_bottle)
    {
        if (current_order->step == current_order->total_step)
        {
            msq_msg_t msg;
            msg.mtype = UPDATE_ORDER_STATUS;
            msg.message.order_status.id_order = *current_order->order->id;
            msg.message.order_status.step = current_order->step;
            msg.message.order_status.total_step = current_order->total_step;
            msg.message.order_status.bottle = current_order->bottle;
            msg.message.order_status.total_bottle = current_order->total_bottle;
            strcpy(msg.message.order_status.message, "Order finished ! Come pick up your order.");

            send_message(msg);

            int new_status = 2;
            update_order(conn, current_order->order, NULL, &new_status);

            GtkBox *current_order_controls = GTK_BOX(gtk_builder_get_object(builder, "current-order-controls"));
            gtk_widget_hide(GTK_WIDGET(current_order_controls));

            GtkAccelLabel *no_active_order = GTK_ACCEL_LABEL(gtk_accel_label_new("No active order"));
            gtk_box_pack_start(current_order_box, GTK_WIDGET(no_active_order), TRUE, TRUE, 0);
            gtk_widget_show(GTK_WIDGET(no_active_order));
            current_order = NULL;
            return;
        }
        else
        {
            int nb_steps;
            current_order->current_cocktail_steps = get_cocktail_steps(conn, &nb_steps, current_order->order->cocktails[current_order->cocktail]->id);

            current_order->bottle = 1;
            current_order->total_bottle = nb_steps;
            current_order->cocktail++;
        }
    }
    else
    {
        current_order->bottle++;
    }

    current_order->step++;

    char *str = malloc(100);
    sprintf(str, "Cocktail %d/%d, Step %d/%d", current_order->cocktail, current_order->total_cocktail, current_order->bottle, current_order->total_bottle);
    GtkLabel *order_title = GTK_LABEL(gtk_label_new(str));

    gtk_box_pack_start(current_order_box, GTK_WIDGET(order_title), TRUE, TRUE, 0);

    sprintf(str, "Bottle: %s", current_order->current_cocktail_steps[current_order->bottle - 1]->bottle->name);
    GtkLabel *step_bottle = GTK_LABEL(gtk_label_new(str));
    gtk_box_pack_start(current_order_box, GTK_WIDGET(step_bottle), TRUE, TRUE, 0);

    sprintf(str, "Quantity: %f", current_order->current_cocktail_steps[current_order->bottle - 1]->quantity);
    GtkLabel *step_quantity = GTK_LABEL(gtk_label_new(str));
    gtk_box_pack_start(current_order_box, GTK_WIDGET(step_quantity), TRUE, TRUE, 0);

    sprintf(str, "Description:\n%s", current_order->current_cocktail_steps[current_order->bottle - 1]->description);
    GtkLabel *step_description = GTK_LABEL(gtk_label_new(str));
    gtk_box_pack_start(current_order_box, GTK_WIDGET(step_description), TRUE, TRUE, 0);

    gtk_widget_set_halign(GTK_WIDGET(step_bottle), GTK_ALIGN_START);
    gtk_widget_set_halign(GTK_WIDGET(step_quantity), GTK_ALIGN_START);
    gtk_widget_set_halign(GTK_WIDGET(step_description), GTK_ALIGN_START);

    gtk_widget_set_margin_start(GTK_WIDGET(current_order_box), 50);
    gtk_widget_show_all(GTK_WIDGET(current_order_box));

    msq_msg_t msg;
    msg.mtype = UPDATE_ORDER_STATUS;
    msg.message.order_status.id_order = *current_order->order->id;
    msg.message.order_status.cocktail = current_order->cocktail;
    msg.message.order_status.total_cocktail = current_order->total_cocktail;
    msg.message.order_status.step = current_order->step;
    msg.message.order_status.total_step = current_order->total_step;
    msg.message.order_status.bottle = current_order->bottle;
    msg.message.order_status.total_bottle = current_order->total_bottle;

    if (current_order->bottle == 1)
    {
        if (current_order->cocktail == 1)
        {
            sprintf(msg.message.order_status.message, "Order taken ! Now preparing %s", current_order->order->cocktails[current_order->cocktail - 1]->name);
        }
        else
        {
            sprintf(msg.message.order_status.message, "Cocktail %s finished ! Now preparing %s", current_order->order->cocktails[current_order->cocktail - 2]->name, current_order->order->cocktails[current_order->cocktail - 1]->name);
        }
    }
    else
    {
        sprintf(msg.message.order_status.message, "The %s has just been poured !", current_order->current_cocktail_steps[current_order->bottle - 2]->bottle->name);
    }

    send_message(msg);
}

GtkWidget *make_order_item(order_t *order)
{
    char *str = malloc(100);
    sprintf(str, "Order n°%lld\n%d cocktail(s)", *order->id, order->nb_cocktails);

    GtkBox *order_item = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    GtkLabel *order_label = GTK_LABEL(gtk_label_new(str));
    GtkButton *order_button = GTK_BUTTON(gtk_button_new_with_label("Start order"));

    gtk_label_set_xalign(order_label, 0);

    gtk_widget_set_size_request(GTK_WIDGET(order_button), 50, -1);
    gtk_widget_set_halign(GTK_WIDGET(order_button), GTK_ALIGN_END);

    g_object_set_data(G_OBJECT(order_button), "order", order);

    g_signal_connect_data(order_button, "clicked", G_CALLBACK(start_order_clicked), NULL, NULL, 0);

    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(order_item));
    gtk_style_context_add_class(context, "order-item");

    gtk_box_pack_start(order_item, GTK_WIDGET(order_label), TRUE, TRUE, 0);
    gtk_box_pack_start(order_item, GTK_WIDGET(order_button), TRUE, TRUE, 0);
    gtk_widget_show_all(GTK_WIDGET(order_item));

    return GTK_WIDGET(order_item);
}

GtkWidget *make_cocktail_item(cocktail_t *cocktail)
{
    char *str = malloc(100);
    sprintf(str, "%s\n%.2f€", cocktail->name, cocktail->price);

    GtkBox *order_item = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    GtkLabel *order_label = GTK_LABEL(gtk_label_new(str));
    GtkButton *order_button = GTK_BUTTON(gtk_button_new_with_label("See information"));

    gtk_label_set_xalign(order_label, 0);

    gtk_widget_set_size_request(GTK_WIDGET(order_button), 50, -1);
    gtk_widget_set_halign(GTK_WIDGET(order_button), GTK_ALIGN_END);

    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(order_item));
    gtk_style_context_add_class(context, "order-item");

    gtk_box_pack_start(order_item, GTK_WIDGET(order_label), TRUE, TRUE, 0);
    gtk_box_pack_start(order_item, GTK_WIDGET(order_button), TRUE, TRUE, 0);

    return GTK_WIDGET(order_item);
}

GtkWidget *make_bottle_item(bottle_t *bottle)
{
    GtkBox *order_item = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    GtkLabel *order_label = GTK_LABEL(gtk_label_new(bottle->name));

    gtk_label_set_xalign(order_label, 0);

    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(order_item));
    gtk_style_context_add_class(context, "order-item");

    gtk_box_pack_start(order_item, GTK_WIDGET(order_label), TRUE, TRUE, 0);

    GtkButton *order_button = GTK_BUTTON(gtk_button_new_with_label("See information"));
    gtk_widget_set_size_request(GTK_WIDGET(order_button), 50, -1);
    gtk_widget_set_halign(GTK_WIDGET(order_button), GTK_ALIGN_END);
    gtk_box_pack_start(order_item, GTK_WIDGET(order_button), TRUE, TRUE, 0);

    return GTK_WIDGET(order_item);
}

GtkWidget *make_bottle_item_addcocktail(step_data_t *step_data)
{
    GtkBox *bottle_item = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    GtkLabel *bottle_label = GTK_LABEL(gtk_label_new(step_data->bottle->name));

    gtk_label_set_xalign(bottle_label, 0);

    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(bottle_item));
    gtk_style_context_add_class(context, "bottle-item");

    gtk_box_pack_start(bottle_item, GTK_WIDGET(bottle_label), TRUE, TRUE, 0);

    GtkBox *controls_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

    GtkWidget *check_bottle = gtk_check_button_new();

    step_data_t *data = step_data;
    g_object_set_data(G_OBJECT(bottle_item), "step_data", data);

    g_signal_connect_data(check_bottle, "clicked", G_CALLBACK(check_bottle_clicked), NULL, NULL, 0);

    gtk_widget_set_name(GTK_WIDGET(check_bottle), step_data->bottle->name);
    gtk_box_pack_start(controls_box, GTK_WIDGET(check_bottle), FALSE, FALSE, 0);
    gtk_box_pack_start(bottle_item, GTK_WIDGET(controls_box), FALSE, FALSE, 0);

    return GTK_WIDGET(bottle_item);
}

GtkWidget *make_buttons_box()
{
    GtkBox *buttons_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

    GtkButton *up_button = GTK_BUTTON(gtk_button_new_with_label("▲"));
    GtkButton *down_button = GTK_BUTTON(gtk_button_new_with_label("▼"));

    gtk_widget_set_name(GTK_WIDGET(buttons_box), "buttons_box");

    g_signal_connect_data(GTK_WIDGET(up_button), "clicked", G_CALLBACK(control_button_bottle_clicked), (gpointer)(uintptr_t)1, NULL, 0);
    g_signal_connect_data(GTK_WIDGET(down_button), "clicked", G_CALLBACK(control_button_bottle_clicked), (gpointer)(uintptr_t)-1, NULL, 0);

    gtk_box_pack_start(buttons_box, GTK_WIDGET(up_button), FALSE, FALSE, 0);
    gtk_box_pack_start(buttons_box, GTK_WIDGET(down_button), FALSE, FALSE, 0);

    return GTK_WIDGET(buttons_box);
}

cocktail_t *get_cocktail_info()
{
    cocktail_t *cocktail = (cocktail_t *)malloc(sizeof(cocktail_t));

    GtkEntry *pCocktailName = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_name"));
    GtkEntry *pCocktailPrice = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_price"));
    GtkEntry *pCocktailDesc = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_description"));
    const char *cocktail_name = gtk_entry_get_text(pCocktailName);
    const char *cocktail_price_txt = gtk_entry_get_text(pCocktailPrice);
    const char *cocktail_desc = gtk_entry_get_text(pCocktailDesc);
    if (strcmp(cocktail_price_txt, "") == 0)
        cocktail_price_txt = "0";

    float cocktail_price = atof(cocktail_price_txt);

    strcpy(cocktail->name, cocktail_name);
    strcpy(cocktail->description, cocktail_desc);
    cocktail->price = cocktail_price;

    return cocktail;
}

void update_step_info(step_data_t *step_data)
{
    GtkLabel *step_label = GTK_LABEL(gtk_builder_get_object(builder, "step_label"));
    GtkEntry *step_quantity = GTK_ENTRY(gtk_builder_get_object(builder, "step_quantity"));
    GtkEntry *step_description = GTK_ENTRY(gtk_builder_get_object(builder, "step_description"));

    char *str = malloc(100);
    sprintf(str, "Step %d/%d  -  Bottle : %s", step_data->position + 1, nb_step, step_data->bottle->name);
    g_print("avant set text\n");
    gtk_label_set_text(step_label, str);

    if (step_data->step_completed == 0)
    {
        gtk_entry_set_text(step_quantity, "0,00");
        gtk_entry_set_text(step_description, "");
    }
    else
    {
        char *quantity = malloc(100);
        sprintf(quantity, "%.2f", step_data->step->quantity);
        gtk_entry_set_text(step_quantity, quantity);
        gtk_entry_set_text(step_description, step_data->step->description);
    }
}

void save_step_info(step_data_t *step_data)
{
    GtkEntry *step_quantity = GTK_ENTRY(gtk_builder_get_object(builder, "step_quantity"));
    GtkEntry *step_description = GTK_ENTRY(gtk_builder_get_object(builder, "step_description"));

    const char *quantity = gtk_entry_get_text(step_quantity);
    const char *description = gtk_entry_get_text(step_description);
    float quantity_float = atof(quantity);

    if (step_data->step_completed == 0)
    {
        step_data->step = (step_t *)malloc(sizeof(step_t));
    }
    step_data->step->quantity = quantity_float;
    strcpy(step_data->step->description, description);
    step_data->step->bottle = step_data->bottle;
    step_data->step_completed = 1;
}

void update_cocktail_list()
{
    int length;
    GtkBox *cocktails_list = GTK_BOX(gtk_builder_get_object(builder, "cocktails-list"));
    cocktail_t **cocktails = get_cocktails(conn, &length);

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(cocktails_list));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    for (int i = 0; i < length; i++)
    {
        if (!cocktails[i]->personalized)
        {
            gtk_box_pack_start(cocktails_list, GTK_WIDGET(make_cocktail_item(cocktails[i])), TRUE, TRUE, 0);
        }
    }
    gtk_widget_show_all(GTK_WIDGET(cocktails_list));
}

GtkWidget *make_module_item(module_t *module)
{
    char *str = malloc(100);
    sprintf(str, "Module - %s\n", module->mac_address);

    GtkBox *module_item = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkBox *header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    GtkBox *non_associated_bottles_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));;

    // Define header
    GtkLabel *header_label = GTK_LABEL(gtk_label_new(str));
    char *label = is_associated(conn, module) ? "Dissociate" : "Hide bottles";
    GtkButton *header_button = GTK_BUTTON(gtk_button_new_with_label(label));
    gtk_label_set_xalign(header_label, 0);
    gtk_widget_set_size_request(GTK_WIDGET(header_button), 50, -1);
    gtk_widget_set_halign(GTK_WIDGET(header_button), GTK_ALIGN_END);
    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(module_item));
    gtk_style_context_add_class(context, "module-item");
    gtk_box_pack_start(header_box, GTK_WIDGET(header_label), TRUE, TRUE, 0);
    gtk_box_pack_start(header_box, GTK_WIDGET(header_button), TRUE, TRUE, 0);

    gtk_box_pack_start(module_item, GTK_WIDGET(header_box), TRUE, TRUE, 0);

    int is_module_associated = is_associated(conn, module);

    if (is_module_associated){
        g_signal_connect_data(header_button, "clicked", G_CALLBACK(dissociate_module_clicked), module->mac_address, NULL, 0);
    }else{
        int nb_bottles;
        bottle_t **bottles = get_non_associated_bottles(conn, &nb_bottles);
        for(int i = 0; i < nb_bottles; i++){
            char *label = (char *)malloc(100 * sizeof(char));
            sprintf(label, "Bottle - %s", bottles[i]->name);
            GtkBox *non_associated_bottle_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
            GtkLabel *non_associated_bottle_label = GTK_LABEL(gtk_label_new(label));
            GtkButton *choose_bottle_button = GTK_BUTTON(gtk_button_new_with_label("Choose"));

            gtk_label_set_xalign(non_associated_bottle_label, 0);
            gtk_widget_set_size_request(GTK_WIDGET(choose_bottle_button), 50, -1);
            gtk_widget_set_halign(GTK_WIDGET(choose_bottle_button), GTK_ALIGN_END);

            // g_signal_connect_data(choose_bottle_button, "clicked", G_CALLBACK(choose_bottle_to_associate), bottles[i]->id, NULL, 0);

            gtk_box_pack_start(non_associated_bottle_box, GTK_WIDGET(non_associated_bottle_label), TRUE, TRUE, 0);
            gtk_box_pack_start(non_associated_bottle_box, GTK_WIDGET(choose_bottle_button), TRUE, TRUE, 0);

            gtk_box_pack_start(non_associated_bottles_box, GTK_WIDGET(non_associated_bottle_box), TRUE, TRUE, 0);
        }

        toggle_button_data_t *data = (toggle_button_data_t *)malloc(sizeof(toggle_button_data_t));
        data->non_associated_bottles = non_associated_bottles_box;
        data->toggle_button = header_button;

        g_signal_connect_data(header_button, "clicked", G_CALLBACK(associate_module_clicked), data, NULL, 0);
        gtk_box_pack_start(module_item, GTK_WIDGET(non_associated_bottles_box), TRUE, TRUE, 0);
    }

    gtk_widget_show_all(GTK_WIDGET(module_item));

    return GTK_WIDGET(module_item);
}
