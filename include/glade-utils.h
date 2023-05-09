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
   int step_completed;
   GtkBox *bottle_item;
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
 * @param bottle A pointer to the bottle object
 * @return GtkWidget* A pointer to the widget
 */
GtkWidget *make_bottle_item(bottle_t *bottle);

/**
 * @brief Get a Widget object from a step_data object for the add cocktail view
 * @param step_data A pointer to the step_data object
 * @return GtkWidget* A pointer to the widget
 */
GtkWidget *make_bottle_item_addcocktail(step_data_t *step_data);

/**
 * @brief Get a Widget object that contains the controls buttons for the bottles list in the add cocktail view
 * @return GtkWidget* A pointer to the widget
 */
GtkWidget *make_buttons_box();

/**
 * @brief Get cocktail info from the GtkEntry objects in the add cocktail view
 * @return cocktail_t* A pointer to the cocktail object
 */
cocktail_t *get_cocktail_info();

/**
 * @brief Update the step detail on the add cocktail view with the step_data object
 * @param step_data A pointer to the step_data object
 */
void update_step_info(step_data_t *step_data);

/**
 * @brief Save the step detail on the add cocktail view with the step_data object
 * @param step_data A pointer to the step_data object
 */
void save_step_info(step_data_t *step_data);

/**
 * @brief Update the cocktail list in the main view
 */
void update_cocktail_list();

/**
 * @brief Get a Widget object from a module object
 * @param module A pointer to the module object
 * @return GtkWidget* A pointer to the widget
 */
GtkWidget *make_module_item(module_t *module);

#endif