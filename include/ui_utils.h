#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "models.h"

void clear_screen();
void pause_screen();
void print_banner();
void print_customer_banner();
void print_admin_banner();
void print_receipt_terminal(Order order);
void get_masked_password(char *password, int max_len);
void print_menu_table(Item *items, int count, int admin_view);

#endif // UI_UTILS_H
