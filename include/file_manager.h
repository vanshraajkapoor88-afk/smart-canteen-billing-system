#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "models.h"

// File paths
#define MENU_FILE    "data/menu.dat"
#define SALES_FILE   "data/sales.dat"
#define STUDENT_FILE "data/students.dat"

// File Operations
void init_default_data();
int get_all_items(Item *items, int max_items);
int save_item(Item item);
int update_item(Item item);
int delete_item(int id);
int find_item_by_id(int id, Item *found_item);
int update_item_stock(int id, int qty_to_deduct);

// Sales Operations
int get_next_bill_number();
int record_order(Order order);
int get_all_orders(Order *orders, int max_orders);

// Student Loyalty Operations
int find_student(const char *student_id, Student *found_student);
int update_student_points(const char *student_id, const char *name, float points_earned, float points_spent);

#endif // FILE_MANAGER_H
