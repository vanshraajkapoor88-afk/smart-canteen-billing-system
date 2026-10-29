#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/admin.h"
#include "../include/file_manager.h"
#include "../include/ui_utils.h"
#include "../include/web_exporter.h"
#include "../include/colors.h"

void run_admin_portal() {
    clear_screen();
    print_banner();
    print_admin_banner();

    char password[32];
    printf("\n%s[Security Check]%s Enter Admin Password: ", COLOR_YELLOW, COLOR_RESET);
    get_masked_password(password, sizeof(password));

    if (strcmp(password, ADMIN_PASSWORD) != 0) {
        printf("%s[Access Denied] Incorrect admin password!%s\n", COLOR_RED, COLOR_RESET);
        pause_screen();
        return;
    }

    printf("%s[Access Granted] Welcome to Admin Control Console.%s\n", COLOR_GREEN, COLOR_RESET);

    int choice;
    do {
        printf("\n%s================ ADMIN CONTROL PANEL ================%s\n", COLOR_YELLOW, COLOR_RESET);
        printf("1. View All Food Items (Inventory)\n");
        printf("2. Add New Food Item\n");
        printf("3. Update Item Price and Stock\n");
        printf("4. Delete Food Item (Soft Delete)\n");
        printf("5. View Low-Stock Warnings (< 5 units)\n");
        printf("6. View Daily Sales & Best-Seller Analytics\n");
        printf("%s7. [SMART FEATURE] Compile & Launch Web Dashboard (HTML)%s\n", COLOR_CYAN, COLOR_RESET);
        printf("0. Return to Main Menu\n");
        printf("%sChoose an option (0-7): %s", COLOR_BOLD, COLOR_RESET);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("%sInvalid input!%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }

        switch (choice) {
            case 1: {
                clear_screen();
                print_admin_banner();
                Item items[100];
                int count = get_all_items(items, 100);
                print_menu_table(items, count, 1);
                pause_screen();
                break;
            }
            case 2: {
                clear_screen();
                print_admin_banner();
                Item new_item;
                printf("\n--- Add New Food Item ---\n");
                printf("Enter Unique Item ID (e.g., 109): ");
                scanf("%d", &new_item.id);

                Item existing;
                if (find_item_by_id(new_item.id, &existing)) {
                    printf("%sAn item with ID %d already exists!%s\n", COLOR_RED, new_item.id, COLOR_RESET);
                    pause_screen();
                    break;
                }

                printf("Enter Item Name: ");
                scanf(" %[^\n]", new_item.name);

                printf("Enter Price (in Rs.): ");
                scanf("%f", &new_item.price);

                printf("Enter Initial Stock Quantity: ");
                scanf("%d", &new_item.stock);

                printf("Choose Category (1:Beverages, 2:Snacks, 3:Meals, 4:Desserts): ");
                scanf("%d", &new_item.category);
                if (new_item.category < 1 || new_item.category > 4) new_item.category = 2;

                new_item.is_active = 1;

                if (save_item(new_item)) {
                    printf("%s[Success] Item '%s' added successfully!%s\n", COLOR_GREEN, new_item.name, COLOR_RESET);
                } else {
                    printf("%s[Error] Failed to write item to menu.dat!%s\n", COLOR_RED, COLOR_RESET);
                }
                pause_screen();
                break;
            }
            case 3: {
                clear_screen();
                print_admin_banner();
                Item items[100];
                int count = get_all_items(items, 100);
                print_menu_table(items, count, 1);

                int id;
                printf("\nEnter Item ID to update: ");
                if (scanf("%d", &id) != 1) break;

                Item itm;
                if (!find_item_by_id(id, &itm)) {
                    printf("%sItem ID %d not found!%s\n", COLOR_RED, id, COLOR_RESET);
                    pause_screen();
                    break;
                }

                printf("Current Name  : %s\n", itm.name);
                printf("Current Price : Rs.%.2f\n", itm.price);
                printf("Current Stock : %d units\n", itm.stock);

                printf("\nEnter New Price (or -1 to keep Rs.%.2f): ", itm.price);
                float np;
                scanf("%f", &np);
                if (np > 0) itm.price = np;

                printf("Enter New Total Stock (or -1 to keep %d units): ", itm.stock);
                int ns;
                scanf("%d", &ns);
                if (ns >= 0) itm.stock = ns;

                if (update_item(itm)) {
                    printf("%s[Success] Item ID %d updated successfully!%s\n", COLOR_GREEN, id, COLOR_RESET);
                } else {
                    printf("%s[Error] Failed to update item!%s\n", COLOR_RED, COLOR_RESET);
                }
                pause_screen();
                break;
            }
            case 4: {
                clear_screen();
                print_admin_banner();
                Item items[100];
                int count = get_all_items(items, 100);
                print_menu_table(items, count, 1);

                int id;
                printf("\nEnter Item ID to delete: ");
                if (scanf("%d", &id) != 1) break;

                printf("Are you sure you want to delete item #%d? (1 for Yes, 0 for No): ", id);
                int confirm;
                scanf("%d", &confirm);
                if (confirm == 1) {
                    if (delete_item(id)) {
                        printf("%s[Success] Item ID %d soft-deleted (historical bills remain intact).%s\n", COLOR_GREEN, id, COLOR_RESET);
                    } else {
                        printf("%s[Error] Item not found or failed to delete.%s\n", COLOR_RED, COLOR_RESET);
                    }
                }
                pause_screen();
                break;
            }
            case 5: {
                clear_screen();
                print_admin_banner();
                Item items[100];
                int count = get_all_items(items, 100);

                printf("\n%s--- INVENTORY WARNINGS & LOW-STOCK ALERTS ---%s\n\n", COLOR_YELLOW, COLOR_RESET);
                int alert_count = 0;
                for (int i = 0; i < count; i++) {
                    if (!items[i].is_active) continue;
                    if (items[i].stock == 0) {
                        printf("%s[SOLD OUT ALERT]%s Item #%d '%s' is out of stock!\n",
                               COLOR_RED, COLOR_RESET, items[i].id, items[i].name);
                        alert_count++;
                    } else if (items[i].stock < 5) {
                        printf("%s[LOW STOCK WARNING]%s Item #%d '%s' has only %d units left!\n",
                               COLOR_YELLOW, COLOR_RESET, items[i].id, items[i].name, items[i].stock);
                        alert_count++;
                    }
                }

                if (alert_count == 0) {
                    printf("%sAll active items have healthy stock levels (>= 5 units).%s\n", COLOR_GREEN, COLOR_RESET);
                }
                pause_screen();
                break;
            }
            case 6: {
                clear_screen();
                print_admin_banner();
                Order orders[200];
                int count = get_all_orders(orders, 200);

                printf("\n%s--- DAILY SALES & REVENUE ANALYTICS ---%s\n\n", COLOR_CYAN, COLOR_RESET);
                float total_revenue = 0.0f;
                float total_gst = 0.0f;
                float total_discounts = 0.0f;

                for (int i = 0; i < count; i++) {
                    total_revenue += orders[i].grand_total;
                    total_gst += orders[i].gst;
                    total_discounts += orders[i].discount;
                }

                printf("  Total Orders Completed  : %s%d%s\n", COLOR_BOLD, count, COLOR_RESET);
                printf("  Total Gross Revenue     : %sRs.%.2f%s\n", COLOR_GREEN, total_revenue, COLOR_RESET);
                printf("  Total GST Collected     : Rs.%.2f\n", total_gst);
                printf("  Total Discounts Given   : Rs.%.2f\n", total_discounts);
                pause_screen();
                break;
            }
            case 7: {
                clear_screen();
                print_admin_banner();
                printf("\n%sGenerating Real-Time Web Dashboard from binary data...%s\n", COLOR_CYAN, COLOR_RESET);
                export_and_launch_web_dashboard();
                pause_screen();
                break;
            }
            case 0:
                printf("\nReturning to Main Menu...\n");
                break;
            default:
                printf("%sInvalid choice!%s\n", COLOR_RED, COLOR_RESET);
        }
    } while (choice != 0);
}
