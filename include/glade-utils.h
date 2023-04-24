#ifndef GLADEUTILE_H
#define GLADEUTILE_H

#include <gtk-3.0/gtk/gtk.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <db-utils/dbutils.h>
#include "gtk-signals-handlers.h"
#include "screen.h"

typedef struct
{
   bottle_t *bottle;
   step_t *step;
   int checked;
   int position;
} step_data_t;

/**
 * @brief Get a Widget object from an order object
 * @param cocktail A pointer to the order object
 * @return GtkWidget* A pointer to the widget
*/
GtkWidget *make_order_item(order_t *order);

/**
 * @brief Get a Widget object from a cocktail object
 * @param cocktail A pointer to the cocktail object
 * @return GtkWidget* A pointer to the widget
*/
GtkWidget *make_cocktail_item(cocktail_t *cocktail);

/**
 * @brief Get a Widget object from a bottle object
 * @param cocktail A pointer to the bottle object
 * @return GtkWidget* A pointer to the widget
*/
GtkWidget *make_bottle_item(bottle_t *bottle);


GtkWidget *make_bottle_item_addcocktail(step_data_t *step_data);

#endif