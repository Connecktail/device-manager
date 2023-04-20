#ifndef GTK_SIGNALS_HANDLERS_H
#define GTK_SIGNALS_HANDLERS_H

#include <gtk-3.0/gtk/gtk.h>

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

#endif