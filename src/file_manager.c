#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/file_manager.h"
#include "../include/colors.h"

// Seeds initial default menu, student accounts, and sample orders if files do not exist
void init_default_data() {
    FILE *fp = fopen(MENU_FILE, "rb");
    if (!fp) {
        fp = fopen(MENU_FILE, "wb");
        if (fp) {
            Item defaults[] = {
                {101, "Masala Dosa", 60.0f, 30, CAT_MEALS, 1},
                {102, "Crispy Veg Samosa", 15.0f, 45, CAT_SNACKS, 1},
                {103, "Hot Masala Chai", 15.0f, 60, CAT_BEVERAGES, 1},
                {104, "Cold Coffee Frappe", 45.0f, 25, CAT_BEVERAGES, 1},
                {105, "Paneer Kathi Roll", 80.0f, 15, CAT_MEALS, 1},
                {106, "Grilled Cheese Sandwich", 50.0f, 4, CAT_SNACKS, 1}, // Low stock (<5) for demo
                {107, "Choco Lava Cake", 65.0f, 0, CAT_DESSERTS, 1},     // Sold out (0) for demo
                {108, "Gulab Jamun (2 pcs)", 40.0f, 18, CAT_DESSERTS, 1}
            };
            int count = sizeof(defaults) / sizeof(defaults[0]);
            fwrite(defaults, sizeof(Item), count, fp);
            fclose(fp);
        }
    } else {
        fclose(fp);
    }

    // Initialize student loyalty data if missing
    FILE *sfp = fopen(STUDENT_FILE, "rb");
    if (!sfp) {
        sfp = fopen(STUDENT_FILE, "wb");
        if (sfp) {
            Student seed_students[] = {
                {"2026CSE001", "Aarav Sharma", 150.0f},
                {"2026CSE042", "Sneha Patel", 85.0f},
                {"2026CSE108", "Rohan Verma", 45.0f}
            };
            fwrite(seed_students, sizeof(Student), 3, sfp);
            fclose(sfp);
        }
    } else {
        fclose(sfp);
    }

    // Initialize sample order history if missing
    FILE *ofp = fopen(SALES_FILE, "rb");
    if (!ofp) {
        ofp = fopen(SALES_FILE, "wb");
        if (ofp) {
            Order sample_order = {
                .bill_number = 1001,
                .student_id = "2026CSE001",
                .date_time = "2026-10-29 12:45",
                .item_count = 2,
                .items = {
                    {101, "Masala Dosa", 60.0f, 2, 120.0f},
                    {103, "Hot Masala Chai", 15.0f, 2, 30.0f}
                },
                .subtotal = 150.0f,
                .discount = 20.0f,
                .gst = 6.5f,
                .grand_total = 136.5f,
                .payment_mode = "UPI"
            };
            fwrite(&sample_order, sizeof(Order), 1, ofp);
            fclose(ofp);
        }
    } else {
        fclose(ofp);
    }
}

int get_all_items(Item *items, int max_items) {
    FILE *fp = fopen(MENU_FILE, "rb");
    if (!fp) return 0;

    int count = 0;
    while (count < max_items && fread(&items[count], sizeof(Item), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    return count;
}

int save_item(Item item) {
    FILE *fp = fopen(MENU_FILE, "ab");
    if (!fp) return 0;
    size_t written = fwrite(&item, sizeof(Item), 1, fp);
    fclose(fp);
    return (written == 1);
}

int update_item(Item updated) {
    FILE *fp = fopen(MENU_FILE, "rb+");
    if (!fp) return 0;

    Item current;
    int found = 0;
    while (fread(&current, sizeof(Item), 1, fp) == 1) {
        if (current.id == updated.id) {
            fseek(fp, -((long)sizeof(Item)), SEEK_CUR);
            fwrite(&updated, sizeof(Item), 1, fp);
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

int delete_item(int id) {
    FILE *fp = fopen(MENU_FILE, "rb+");
    if (!fp) return 0;

    Item current;
    int found = 0;
    while (fread(&current, sizeof(Item), 1, fp) == 1) {
        if (current.id == id) {
            current.is_active = 0; // Soft delete
            fseek(fp, -((long)sizeof(Item)), SEEK_CUR);
            fwrite(&current, sizeof(Item), 1, fp);
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

int find_item_by_id(int id, Item *found_item) {
    FILE *fp = fopen(MENU_FILE, "rb");
    if (!fp) return 0;

    Item temp;
    int found = 0;
    while (fread(&temp, sizeof(Item), 1, fp) == 1) {
        if (temp.id == id && temp.is_active) {
            *found_item = temp;
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

int update_item_stock(int id, int qty_to_deduct) {
    FILE *fp = fopen(MENU_FILE, "rb+");
    if (!fp) return 0;

    Item current;
    int success = 0;
    while (fread(&current, sizeof(Item), 1, fp) == 1) {
        if (current.id == id && current.is_active) {
            if (current.stock >= qty_to_deduct) {
                current.stock -= qty_to_deduct;
                fseek(fp, -((long)sizeof(Item)), SEEK_CUR);
                fwrite(&current, sizeof(Item), 1, fp);
                success = 1;
            }
            break;
        }
    }
    fclose(fp);
    return success;
}

int get_next_bill_number() {
    FILE *fp = fopen(SALES_FILE, "rb");
    if (!fp) return 1001;

    Order last_order;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    if (size < (long)sizeof(Order)) {
        fclose(fp);
        return 1001;
    }

    fseek(fp, -((long)sizeof(Order)), SEEK_END);
    if (fread(&last_order, sizeof(Order), 1, fp) == 1) {
        fclose(fp);
        return last_order.bill_number + 1;
    }
    fclose(fp);
    return 1001;
}

int record_order(Order order) {
    FILE *fp = fopen(SALES_FILE, "ab");
    if (!fp) return 0;
    size_t written = fwrite(&order, sizeof(Order), 1, fp);
    fclose(fp);
    return (written == 1);
}

int get_all_orders(Order *orders, int max_orders) {
    FILE *fp = fopen(SALES_FILE, "rb");
    if (!fp) return 0;

    int count = 0;
    while (count < max_orders && fread(&orders[count], sizeof(Order), 1, fp) == 1) {
        count++;
    }
    fclose(fp);
    return count;
}

int find_student(const char *student_id, Student *found_student) {
    FILE *fp = fopen(STUDENT_FILE, "rb");
    if (!fp) return 0;

    Student temp;
    int found = 0;
    while (fread(&temp, sizeof(Student), 1, fp) == 1) {
        if (strcmp(temp.student_id, student_id) == 0) {
            *found_student = temp;
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

int update_student_points(const char *student_id, const char *name, float points_earned, float points_spent) {
    FILE *fp = fopen(STUDENT_FILE, "rb+");
    if (!fp) {
        // Create if missing
        fp = fopen(STUDENT_FILE, "wb+");
        if (!fp) return 0;
    }

    Student current;
    int found = 0;
    while (fread(&current, sizeof(Student), 1, fp) == 1) {
        if (strcmp(current.student_id, student_id) == 0) {
            current.loyalty_points = current.loyalty_points - points_spent + points_earned;
            if (current.loyalty_points < 0) current.loyalty_points = 0;
            fseek(fp, -((long)sizeof(Student)), SEEK_CUR);
            fwrite(&current, sizeof(Student), 1, fp);
            found = 1;
            break;
        }
    }

    if (!found) {
        // Register new student automatically
        Student new_student;
        strncpy(new_student.student_id, student_id, sizeof(new_student.student_id) - 1);
        strncpy(new_student.name, name, sizeof(new_student.name) - 1);
        new_student.loyalty_points = points_earned - points_spent;
        if (new_student.loyalty_points < 0) new_student.loyalty_points = 0;
        fseek(fp, 0, SEEK_END);
        fwrite(&new_student, sizeof(Student), 1, fp);
    }

    fclose(fp);
    return 1;
}
