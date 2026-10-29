#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>
#endif
#include "../include/ui_utils.h"
#include "../include/colors.h"

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause_screen() {
    printf("\n%sPress [Enter] to continue...%s", COLOR_GRAY, COLOR_RESET);
    while (getchar() != '\n');
    getchar();
}

void print_banner() {
    printf("%s", COLOR_CYAN);
    printf("========================================================================\n");
    printf("  ____  __  __    _    ____ _____   ____    _    _   _ _____ _____ _____ \n");
    printf(" / ___||  \\/  |  / \\  |  _ \\_   _| / ___|  / \\  | \\ | |_   _| ____| ____|\n");
    printf(" \\___ \\| |\\/| | / _ \\ | |_) || |  | |     / _ \\ |  \\| | | | |  _| |  _|  \n");
    printf("  ___) | |  | |/ ___ \\|  _ < | |  | |___ / ___ \\| |\\  | | | | |___| |___ \n");
    printf(" |____/|_|  |_/_/   \\_\\_| \\_\\|_|   \\____/_/   \\_\\_| \\_| |_| |_____|_____|\n");
    printf("                 B.Tech CSE Hackathon POS & Billing System             \n");
    printf("========================================================================%s\n", COLOR_RESET);
}

void print_customer_banner() {
    printf("%s", COLOR_GREEN);
    printf("------------------------------------------------------------------------\n");
    printf("                   CUSTOMER SELF-SERVICE ORDER PORTAL                   \n");
    printf("------------------------------------------------------------------------%s\n", COLOR_RESET);
}

void print_admin_banner() {
    printf("%s", COLOR_YELLOW);
    printf("------------------------------------------------------------------------\n");
    printf("                     ADMIN & INVENTORY CONTROL PANEL                    \n");
    printf("------------------------------------------------------------------------%s\n", COLOR_RESET);
}

static const char* cat_to_str(int cat) {
    switch (cat) {
        case CAT_BEVERAGES: return "Beverages";
        case CAT_SNACKS:    return "Snacks";
        case CAT_MEALS:     return "Meals";
        case CAT_DESSERTS:  return "Desserts";
        default:            return "General";
    }
}

void print_menu_table(Item *items, int count, int admin_view) {
    printf("%s+-----+-------------------------------+-------------+---------+---------+--------------+%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s| ID  | Food Item Name                | Category    | Price   | Stock   | Status       |%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s+-----+-------------------------------+-------------+---------+---------+--------------+%s\n", COLOR_CYAN, COLOR_RESET);

    for (int i = 0; i < count; i++) {
        if (!items[i].is_active && !admin_view) continue;

        char status_str[24];
        char color[16] = COLOR_GREEN;

        if (!items[i].is_active) {
            snprintf(status_str, sizeof(status_str), "DELETED");
            strcpy(color, COLOR_GRAY);
        } else if (items[i].stock == 0) {
            snprintf(status_str, sizeof(status_str), "SOLD OUT");
            strcpy(color, COLOR_RED);
        } else if (items[i].stock < 5) {
            snprintf(status_str, sizeof(status_str), "LOW STOCK");
            strcpy(color, COLOR_YELLOW);
        } else {
            snprintf(status_str, sizeof(status_str), "IN STOCK");
            strcpy(color, COLOR_GREEN);
        }

        printf("| %-3d | %-29s | %-11s | Rs.%-5.2f | %-7d | %s%-12s%s |\n",
               items[i].id,
               items[i].name,
               cat_to_str(items[i].category),
               items[i].price,
               items[i].stock,
               color,
               status_str,
               COLOR_RESET);
    }
    printf("%s+-----+-------------------------------+-------------+---------+---------+--------------+%s\n", COLOR_CYAN, COLOR_RESET);
}

void print_receipt_terminal(Order order) {
    printf("\n%s========================================================================\n", COLOR_CYAN);
    printf("                       CAMPUS SMART CANTEEN RECEIPT                     \n");
    printf("========================================================================%s\n", COLOR_RESET);
    printf(" Bill Number : %s#%d%s\t\tDate/Time: %s\n", COLOR_YELLOW, order.bill_number, COLOR_RESET, order.date_time);
    printf(" Customer ID : %s\t\tPayment Mode: %s\n", order.student_id, order.payment_mode);
    printf("------------------------------------------------------------------------\n");
    printf(" %-4s | %-32s | %-5s | %-10s | %-10s\n", "S.No", "Item Description", "Qty", "Rate (Rs)", "Amount (Rs)");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < order.item_count; i++) {
        printf(" %-4d | %-32s | %-5d | %-10.2f | %-10.2f\n",
               i + 1,
               order.items[i].name,
               order.items[i].quantity,
               order.items[i].unit_price,
               order.items[i].total_price);
    }
    printf("------------------------------------------------------------------------\n");
    printf(" %54s: Rs.%8.2f\n", "Subtotal", order.subtotal);
    if (order.discount > 0.0f) {
        printf(" %s%54s: -Rs.%7.2f%s\n", COLOR_GREEN, "Discount (Loyalty/Coupon)", order.discount, COLOR_RESET);
    }
    printf(" %54s: Rs.%8.2f\n", "GST (5%)", order.gst);
    printf("%s========================================================================%s\n", COLOR_CYAN, COLOR_RESET);
    printf(" %s%54s: Rs.%8.2f%s\n", COLOR_BOLD, "GRAND TOTAL", order.grand_total, COLOR_RESET);
    printf("%s========================================================================%s\n", COLOR_CYAN, COLOR_RESET);
    printf("        %sThank you for dining with us! E-Receipt generated.%s\n", COLOR_GREEN, COLOR_RESET);
    printf("========================================================================\n\n");
}

void get_masked_password(char *password, int max_len) {
    int i = 0;
#ifdef _WIN32
    char ch;
    while (1) {
        ch = _getch();
        if (ch == 13 || ch == 10) { // Enter key
            break;
        } else if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < max_len - 1 && ch >= 32 && ch <= 126) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
#else
    scanf("%s", password);
#endif
}
