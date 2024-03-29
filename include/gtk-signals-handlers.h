#ifndef GTK_SIGNALS_HANDLERS_H
#define GTK_SIGNALS_HANDLERS_H

#include <gtk-3.0/gtk/gtk.h>
#include <db-utils/dbutils.h>
#include "glade-utils.h"

/**
 * @brief Show the modal to scan a bottle
 * @note handler fo the button "Scan a new bottle"
 */
void show_scan_bottle_modal();

/**
 * @brief Hide the modal to scan a bottle
 * @note handler fo the button "Cancel"
 */
void hide_scan_bottle_modal();

/**
 * @brief Function to validate the barcode and send the barcode and price in message queue
 * @param button
 * @param user_data
 * @note handler fo the button "Validate"
 * @note This function waits for the execution of the get_bottle_price function via a semaphore
 */
void validate_barcode(GtkButton *button, gpointer user_data);

/**
 * @brief Function to get the bottle price
 * @param button  The button that triggered the event
 * @param user_data The data passed to the function
 * @note handler fo the button "Validate"
 */
void get_bottle_price(GtkButton *button, gpointer user_data);

/**
 * @brief Function to handle check on the bottle list in the new cocktail modal
 * @param button  The button that triggered the event
 * @note handler for the checkbox on the bottle list
 */
void check_bottle_clicked(GtkButton *button);

/**
 * @brief Function to handle check on the bottle list in the new cocktail modal
 * @param button  The button that triggered the event
 * @param b_data The data passed to the function
 * @note handler for the button "▲" and "▼" on the bottle list
 */
void control_button_bottle_clicked(GtkButton *button, gpointer b_data);

/**
 * @brief Function to associate a module
 * @param button  The button that triggered the event
 * @param b_data The data passed to the function
 * @note handle for the associate button
 */
void associate_module(GtkButton *button, gpointer b_data);

/**
 * @brief Function to handle check on the "Start order" buttons
 * @param button  The button that triggered the event
 */
void start_order_clicked(GtkButton *button);

/**
 * @brief Function to display the non associated bottles
 * @param button  The button that triggered the event
 * @param b_data The data passed to the function
 * @note handle for the associate button, acts as a toggle button
 */
void associate_module_clicked(GtkButton *button, gpointer b_data);

/**
 * @brief Function to dissociate a module
 * @param button  The button that triggered the event
 * @param b_data The data passed to the function
 * @note handle for the dissociate button
 */
void dissociate_module_clicked(GtkButton *button, gpointer b_data);

/**
 * @brief Function to handle check on the "Start order" buttons
 * @param button  The button that triggered the event
 */
void start_order_clicked(GtkButton *button);

/**
 * @brief Function send a led_message_t to the message queue
 * @note handler for the button "Find"
*/
void light_led();

/**
 * @brief Function to associate a bottle with a module
 * @param button  The button that triggered the event
 * @param b_data The data passed to the function : struct containing the bottle and the module
 * @note handle for the choose button
 */
void choose_bottle_to_associate(GtkButton *button, gpointer b_data);

/**
 * @brief Function to play a sound on the module of the current order
 * @note handler for the button "Play sound"
*/
void play_sound();

#endif