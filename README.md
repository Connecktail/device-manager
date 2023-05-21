# Device manager

## Purpose :

This process includes all the UI needed to the Connecktail projet. It contains the possibility to scan a bootle, pair a new module, associate a module with a bottle, handle orders...

## Compilation :

### Pre-requisites :

- libpq-dev
- libshm-utils
- libdb-utils
- lib gtk

### Instructions : 

- clone the repository
- move to the root of the project
- execute the following commands :

```bash
make
```

It will create a build folder with the executable inside.

You are now able to launch the process with the following command :

```bash
./build/main
```


## Data structures :

Some structs have been defined in order to store multiple data inside a single variable but aslo pass multiple variables to a callback function.
**Step :**

```c
typedef struct
{
   bottle_t *bottle;
   step_t *step;
   int checked;
   int position;
   int step_completed;
   GtkBox *bottle_item;
} step_data_t;
```

**Associate a module with a bottle**

```c
typedef struct
{
   module_t *module;
   bottle_t *bottle;
} association_data_t;
```

**Show non associated bottles for a given module**

```c
typedef struct
{
   GtkBox *non_associated_bottles;
   GtkButton *toggle_button;
} toggle_button_data_t;
```

**Stock the current order**

```c
typedef struct {
   order_t *order;
   int cocktail;
   int total_cocktail;
   int step;
   int total_step;
   int bottle;
   int total_bottle;
   step_t **current_cocktail_steps;
} current_order_t;
```

## Features : 

### Home page :

The user has at the left of the page, the list of orders that have to be prepared. He has also the possiblity to start that order by clicking on the button "start order". That order will only be started if no other order is currently being prepared.
At the right of the page, the user see his current order with the information of the current step. He can go to the next step, find a bottle by turning on the led or playing a sound.

If the user clicks on the button "Adminsitration", he is redirected to the administration page.

### Administration page :

At the left of the page : 

You can see the list of cocktails created
At the bottom, you have a button to create a new cocktail. By clicking on it, you will have to provide various informations about it and specify the steps of it.

At the right of the page :

You can see the list of bottles that you have scanned
At the bottom, you have a button to scan a bottle. By clicking on it, you will be asked to provide the price of the bottle and the barcode of it bby using the barcode scanner at your disposal.

At the top right hand corner of the page you have a button to pair a new module. Instructions will be displayed on the screen to help you.

At the top middle of the page you have a button to associate a module with a bottle. Instructions will be displayed on the screen to help you.

## Files :

### Bottle taken :

This file contains the code of a thread which will wait for recieving a message indicating that a bottle has been taken. In that case, it will update the current order and the UI ot go to the next step.

```c
void handle_bottle_taken()
```

### Glade utils :

This file contains various functions that will be useful to interact with the UI. For examples there are some functions that return a Widgets from some calls to the database. This include not static content.

### Gtk signal handlers :

This file contains all the handler for event recieved from the user on some buttons, label....

### Scanner : 

This file contains the code of a thread which will wait a click from the user on a button indicating we have to scan a new bottle's barcode. It will then send the barcode and the price of the bottle to the main process via the message queue.

```c
void *handle_scanner(void *arg)
```

### Screen :

This file contains some global functions to go to the adminstration page for example.

- Thread responsible to display the screen app :

```c
void *display_screen(void *arg)
```

- Update the orders list when a signal is recieved or when the user start an order :

```c
gboolean update_orders_list()
```

- Update the bottles list when a signal is recieved :

```c
gboolean update_bottles_list()
```

- Close the app

```c
void close_app()
```

- Go to the admin page

```c
void go_to_admin()
```

- Go to the home page

```c
void go_to_homepage()
```

### Signal handler :

- Handler for unix signals : 

```c
void signal_handler_linux(int signum)
```