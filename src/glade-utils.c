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