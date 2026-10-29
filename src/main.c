#include <stdio.h>
#include <stdlib.h>
#include "../include/models.h"
#include "../include/colors.h"
#include "../include/file_manager.h"
#include "../include/ui_utils.h"
#include "../include/customer.h"
#include "../include/admin.h"
#include "../include/web_exporter.h"

int main() {
    // 1. Initialize Windows ANSI color support
    init_console_colors();

    // 2. Ensure data files are seeded with initial items and students
    init_default_data();

    int choice;
    do {
        clear_screen();
        print_banner();

        printf("\n%s================ MAIN SELECTION MENU ================%s\n", COLOR_CYAN, COLOR_RESET);
        printf(" 1. %sCustomer Self-Service Portal%s (Order Food & Redeem Loyalty)\n", COLOR_GREEN, COLOR_RESET);
        printf(" 2. %sAdmin & Inventory Management%s (Password Protected)\n", COLOR_YELLOW, COLOR_RESET);
        printf(" 3. %s[SMART WEB] Open Live HTML Web Dashboard%s\n", COLOR_CYAN, COLOR_RESET);
        printf(" 4. Exit Application\n");
        printf("======================================================\n");
        printf("%sEnter your choice (1-4): %s", COLOR_BOLD, COLOR_RESET);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("%sInvalid selection! Please enter a number between 1 and 4.%s\n", COLOR_RED, COLOR_RESET);
            pause_screen();
            continue;
        }

        switch (choice) {
            case 1:
                run_customer_portal();
                break;
            case 2:
                run_admin_portal();
                break;
            case 3:
                clear_screen();
                print_banner();
                printf("\n%sCompiling and launching Web Dashboard in your default browser...%s\n", COLOR_CYAN, COLOR_RESET);
                export_and_launch_web_dashboard();
                pause_screen();
                break;
            case 4:
                clear_screen();
                print_banner();
                printf("\n%sThank you for using Smart Canteen POS System! Goodbye.%s\n\n", COLOR_GREEN, COLOR_RESET);
                break;
            default:
                printf("%sInvalid choice! Please choose 1, 2, 3, or 4.%s\n", COLOR_RED, COLOR_RESET);
                pause_screen();
        }
    } while (choice != 4);

    return 0;
}
