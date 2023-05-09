#include "../include/screen.h"

extern PGconn *conn;
extern GtkBuilder *builder;
extern int nb_step;

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

    GtkBox *module_item = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    GtkLabel *module_label = GTK_LABEL(gtk_label_new(str));

    char *label = is_associated(conn, module) ? "Dissociate" : "Associate";
    GtkButton *module_button = GTK_BUTTON(gtk_button_new_with_label(label));
    // if is_associated :
    // g_signal_connect_data(module_button, "clicked", G_CALLBACK(dissociate_module), (gpointer)module->mac_address, NULL, 0);
    // else
    //  g_signal_connect_data(module_button, "clicked", G_CALLBACK(associate_module), (gpointer)(uintptr_t)1, NULL, 0);

    gtk_label_set_xalign(module_label, 0);

    gtk_widget_set_size_request(GTK_WIDGET(module_button), 50, -1);
    gtk_widget_set_halign(GTK_WIDGET(module_button), GTK_ALIGN_END);

    GtkStyleContext *context;
    context = gtk_widget_get_style_context(GTK_WIDGET(module_item));
    gtk_style_context_add_class(context, "module-item");

    gtk_box_pack_start(module_item, GTK_WIDGET(module_label), TRUE, TRUE, 0);
    gtk_box_pack_start(module_item, GTK_WIDGET(module_button), TRUE, TRUE, 0);

    return GTK_WIDGET(module_item);
}
