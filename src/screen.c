#include "../include/screen.h"

PGconn *conn;
order_t **orders;
cocktail_t **cocktails;
bottle_t **bottles;
int length;

GtkBuilder *builder;
GdkScreen *gdk_screen;
GtkWidget *window;

GtkBox *orders_list, *cocktails_list, *bottles_list;
GtkWidget *stack;
GtkWidget *pHomepage, *pAdministration; 

GtkCssProvider *css_provider;

void *display_screen(void *arg) {
	conn = db_connect(db_host, db_database, db_user, db_password);

    struct sigaction newact;
    newact.sa_handler = signal_handler;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGUSR2, &newact, NULL);

    gtk_init(NULL, NULL);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "./glade/screen-app.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    stack = GTK_WIDGET(gtk_builder_get_object(builder, "principal_stack"));
    pHomepage = GTK_WIDGET(gtk_builder_get_object(builder, "homepage_box"));
    pAdministration = GTK_WIDGET(gtk_builder_get_object(builder, "administration_box"));

    orders_list = GTK_BOX(gtk_builder_get_object(builder, "orders-list"));
    cocktails_list = GTK_BOX(gtk_builder_get_object(builder, "cocktails-list"));
    bottles_list = GTK_BOX(gtk_builder_get_object(builder, "bottles-list"));

    gdk_screen = gtk_widget_get_screen(window);

    orders = get_orders(conn, &length);
    for(int i = 0; i < length; i++) {
        gtk_box_pack_start(orders_list, GTK_WIDGET(make_order_item(orders[i])), TRUE, TRUE, 0);
    }
    cocktails = get_cocktails(conn, &length);
    for(int i = 0; i < length; i++) {
        if(!cocktails[i]->personalized) {
            gtk_box_pack_start(cocktails_list, GTK_WIDGET(make_cocktail_item(cocktails[i])), TRUE, TRUE, 0);
        }
    }
    bottles = get_bottles(conn, &length);
    for(int i = 0; i < length; i++) {
        gtk_box_pack_start(bottles_list, GTK_WIDGET(make_bottle_item(bottles[i])), TRUE, TRUE, 0);
    }


    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "./glade/screen-app.css", NULL);
    gtk_style_context_add_provider_for_screen (gdk_screen, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_stack_set_visible_child(GTK_STACK(stack), pHomepage);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return NULL;
}

void signal_handler() {
    g_idle_add(update_screen, NULL);
}

gboolean update_screen() {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(orders_list));
    for(iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    orders = get_orders(conn, &length);
    for(int i = 0; i < length; i++) {
        gtk_box_pack_start(orders_list, GTK_WIDGET(make_order_item(orders[i])), TRUE, TRUE, 0);
    }
    
    gtk_widget_show_all(GTK_WIDGET(orders_list));
    return TRUE;
}

void close_app() {
    gtk_main_quit();
    pthread_exit(NULL);
}

void go_to_admin() {
    gtk_stack_set_visible_child(GTK_STACK(stack), pAdministration);
}
void go_to_homepage() {
    gtk_stack_set_visible_child(GTK_STACK(stack), pHomepage);
}