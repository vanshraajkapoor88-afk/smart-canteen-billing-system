#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "models.h"

// Dynamic Cart Management
Cart* init_cart();
void free_cart(Cart *cart);
int add_to_cart(Cart *cart, int item_id, const char *name, float unit_price, int quantity, int max_stock);
int remove_from_cart(Cart *cart, int item_id);
void view_cart(const Cart *cart);

// Customer Portal Interface
void run_customer_portal();

#endif // CUSTOMER_H
