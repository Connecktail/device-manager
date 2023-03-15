#include "../include/screen.h"

PGconn *conn;

GdkScreen *gdk_screen;
GtkWidget *window;
GtkButton *button;
GtkBox *orders_list;

GtkCssProvider *css_provider;

void *display_screen(void *arg) {
	conn = db_connect(db_host, db_database, db_user, db_password);
    int length;
    order_t **orders = get_orders(conn, &length);
    
    GtkBuilder *builder;
    gtk_init(NULL, NULL);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "../glade/screen-app.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "homepage"));
    orders_list = GTK_BOX(gtk_builder_get_object(builder, "orders-list"));
    gdk_screen = gtk_widget_get_screen(window);

    for(int i = 0; i < length; i++) {
        char *str = malloc(100);
        sprintf(str, "Order n°%d\n%d cocktail(s)", i+1, orders[i]->nb_cocktails);
        
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
        gtk_box_pack_start(orders_list, GTK_WIDGET(order_item), TRUE, TRUE, 0);
    }

    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "../glade/screen-app.css", NULL);
    gtk_style_context_add_provider_for_screen (gdk_screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return NULL;
}

void close_app() {
    gtk_main_quit();
    pthread_exit(NULL);
}