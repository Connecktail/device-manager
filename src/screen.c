#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/screen.h"
#include "../include/gtk-signals-handlers.h"

PGconn *conn;
order_t **orders;
cocktail_t **cocktails;
bottle_t **bottles;
step_data_t **bottle_data_list;
int length, nb_bottles;

int add_cocktail_step = 0;
int nb_step = 0;
cocktail_t *cocktail_added;

GtkBuilder *builder;
GdkScreen *gdk_screen;
GtkWidget *window;

GtkBox *orders_list, *cocktails_list, *bottles_list;
GtkBox *bottles_selection_list;
GtkWidget *stack, *addCocktailStack;
GtkWidget *pHomepage, *pAdministration, *pAddCocktail, *pPairModuleBox;
GtkWidget *pScanBottleModal, *pAddCocktailModal;
GtkWidget *pCocktailInfos, *pBottlesSelection, *pStepInfos;
GtkWidget *pPairModuleModalLabel;
GtkWidget *pValidatePairingButton;

GtkCssProvider *css_provider;

sem_t send_barcode_semaphore;

void *display_screen(void *arg)
{
    conn = db_connect(db_host, db_database, db_user, db_password);

    sem_init(&send_barcode_semaphore, 0, 0);

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
    addCocktailStack = GTK_WIDGET(gtk_builder_get_object(builder, "add_cocktail_stack"));
    pHomepage = GTK_WIDGET(gtk_builder_get_object(builder, "homepage_box"));
    pAdministration = GTK_WIDGET(gtk_builder_get_object(builder, "administration_box"));
    pAddCocktail = GTK_WIDGET(gtk_builder_get_object(builder, "add_cocktail_box"));
    pPairModuleBox = GTK_WIDGET(gtk_builder_get_object(builder, "pair_module_box"));
    pScanBottleModal = GTK_WIDGET(gtk_builder_get_object(builder, "scan_bottle_modal"));
    pAddCocktailModal = GTK_WIDGET(gtk_builder_get_object(builder, "add_cocktail_modal"));
    pCocktailInfos = GTK_WIDGET(gtk_builder_get_object(builder, "cocktail_infos"));
    pBottlesSelection = GTK_WIDGET(gtk_builder_get_object(builder, "bottles_selection"));
    pStepInfos = GTK_WIDGET(gtk_builder_get_object(builder, "step_infos"));

    pPairModuleModalLabel = GTK_WIDGET(gtk_builder_get_object(builder, "pair_module_modal_label"));
    pValidatePairingButton = GTK_WIDGET(gtk_builder_get_object(builder, "validate_pairing_button"));

    orders_list = GTK_BOX(gtk_builder_get_object(builder, "orders-list"));
    cocktails_list = GTK_BOX(gtk_builder_get_object(builder, "cocktails-list"));
    bottles_list = GTK_BOX(gtk_builder_get_object(builder, "bottles-list"));
    bottles_selection_list = GTK_BOX(gtk_builder_get_object(builder, "bottles-selection-list"));

    gdk_screen = gtk_widget_get_screen(window);

    GtkEntry *pCocktailName1 = GTK_ENTRY(gtk_builder_get_object(builder, "cocktail_name1"));
    gtk_entry_set_text(pCocktailName1, "blablabal");

    orders = get_orders(conn, &length);
    for (int i = 0; i < length; i++)
    {
        gtk_box_pack_start(orders_list, GTK_WIDGET(make_order_item(orders[i])), TRUE, TRUE, 0);
    }
    cocktails = get_cocktails(conn, &length);
    for (int i = 0; i < length; i++)
    {
        if (!cocktails[i]->personalized)
        {
            gtk_box_pack_start(cocktails_list, GTK_WIDGET(make_cocktail_item(cocktails[i])), TRUE, TRUE, 0);
        }
    }
    bottles = get_bottles(conn, &nb_bottles);
    bottle_data_list = (step_data_t **)malloc(sizeof(step_data_t *) * (nb_bottles));
    for (int i = 0; i < nb_bottles; i++)
    {
        gtk_box_pack_start(bottles_list, GTK_WIDGET(make_bottle_item(bottles[i])), TRUE, TRUE, 0);
    }

    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "./glade/screen-app.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_stack_set_visible_child(GTK_STACK(stack), pHomepage);

    gtk_builder_connect_signals(builder, NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return NULL;
}

void signal_handler()
{
    g_idle_add_full(G_PRIORITY_HIGH_IDLE, update_screen, NULL, NULL);
}

gboolean update_screen()
{
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(orders_list));
    for (iter = children; iter != NULL; iter = g_list_next(iter))
    {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    orders = get_orders(conn, &length);
    for (int i = 0; i < length; i++)
    {
        gtk_box_pack_start(orders_list, GTK_WIDGET(make_order_item(orders[i])), TRUE, TRUE, 0);
    }

    gtk_widget_show_all(GTK_WIDGET(orders_list));

    return G_SOURCE_REMOVE;
}

void close_app()
{
    gtk_main_quit();
    pthread_exit(NULL);
}
