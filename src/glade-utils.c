#include "../include/screen.h"

extern PGconn *conn;

GtkWidget *make_order_item(order_t *order) {
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
    gtk_style_context_add_class(context,"order-item");

    gtk_box_pack_start(order_item, GTK_WIDGET(order_label), TRUE, TRUE, 0);
    gtk_box_pack_start(order_item, GTK_WIDGET(order_button), TRUE, TRUE, 0);
    gtk_widget_show_all(GTK_WIDGET(order_item));

    return GTK_WIDGET(order_item);
}

GtkWidget *make_cocktail_item(cocktail_t *cocktail) {
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
    gtk_style_context_add_class(context,"order-item");

    gtk_box_pack_start(order_item, GTK_WIDGET(order_label), TRUE, TRUE, 0);
    gtk_box_pack_start(order_item, GTK_WIDGET(order_button), TRUE, TRUE, 0);

    return GTK_WIDGET(order_item);
}

GtkWidget *make_bottle_item(bottle_t *bottle) {
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

GtkWidget *make_bottle_item_addcocktail(step_data_t *step_data) {
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