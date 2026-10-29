#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/customer.h"
#include "../include/file_manager.h"
#include "../include/ui_utils.h"
#include "../include/web_exporter.h"
#include "../include/colors.h"

#define INITIAL_CART_CAPACITY 4

Cart* init_cart() {
    Cart *cart = (Cart*)malloc(sizeof(Cart));
    if (!cart) {
        printf("%s[Memory Error] Failed to initialize cart%s\n", COLOR_RED, COLOR_RESET);
        return NULL;
    }
    cart->items = (CartItem*)malloc(INITIAL_CART_CAPACITY * sizeof(CartItem));
    if (!cart->items) {
        free(cart);
        return NULL;
    }
    cart->count = 0;
    cart->capacity = INITIAL_CART_CAPACITY;
    return cart;
}

void free_cart(Cart *cart) {
    if (cart) {
        if (cart->items) {
            free(cart->items);
        }
        free(cart);
    }
}

int add_to_cart(Cart *cart, int item_id, const char *name, float unit_price, int quantity, int max_stock) {
    if (!cart || quantity <= 0) return 0;

    // Check if item is already in cart
    for (int i = 0; i < cart->count; i++) {
        if (cart->items[i].item_id == item_id) {
            if (cart->items[i].quantity + quantity > max_stock) {
                printf("%s[Stock Limit] Cannot add more than %d units total! (Currently in cart: %d)%s\n",
                       COLOR_RED, max_stock, cart->items[i].quantity, COLOR_RESET);
                return 0;
            }
            cart->items[i].quantity += quantity;
            cart->items[i].total_price = cart->items[i].quantity * unit_price;
            return 1;
        }
    }

    // Check if new quantity exceeds stock
    if (quantity > max_stock) {
        printf("%s[Stock Limit] Only %d units available in stock!%s\n", COLOR_RED, max_stock, COLOR_RESET);
        return 0;
    }

    // Dynamic resize if capacity reached
    if (cart->count >= cart->capacity) {
        int new_capacity = cart->capacity * 2;
        CartItem *new_items = (CartItem*)realloc(cart->items, new_capacity * sizeof(CartItem));
        if (!new_items) {
            printf("%s[Memory Error] Could not expand cart memory!%s\n", COLOR_RED, COLOR_RESET);
            return 0;
        }
        cart->items = new_items;
        cart->capacity = new_capacity;
    }

    // Add new item
    cart->items[cart->count].item_id = item_id;
    strncpy(cart->items[cart->count].name, name, sizeof(cart->items[cart->count].name) - 1);
    cart->items[cart->count].unit_price = unit_price;
    cart->items[cart->count].quantity = quantity;
    cart->items[cart->count].total_price = quantity * unit_price;
    cart->count++;
    return 1;
}

int remove_from_cart(Cart *cart, int item_id) {
    if (!cart || cart->count == 0) return 0;

    int found_idx = -1;
    for (int i = 0; i < cart->count; i++) {
        if (cart->items[i].item_id == item_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) return 0;

    // Shift remaining items left
    for (int i = found_idx; i < cart->count - 1; i++) {
        cart->items[i] = cart->items[i + 1];
    }
    cart->count--;
    return 1;
}

void view_cart(const Cart *cart) {
    if (!cart || cart->count == 0) {
        printf("\n%sYour cart is currently empty!%s\n", COLOR_YELLOW, COLOR_RESET);
        return;
    }

    printf("\n%s+-----+-------------------------------+---------+---------+---------------+%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s| S.N | Item Description              | Qty     | Rate    | Total (Rs)    |%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s+-----+-------------------------------+---------+---------+---------------+%s\n", COLOR_CYAN, COLOR_RESET);

    float subtotal = 0.0f;
    for (int i = 0; i < cart->count; i++) {
        printf("| %-3d | %-29s | %-7d | Rs.%-5.2f| Rs.%-10.2f |\n",
               i + 1,
               cart->items[i].name,
               cart->items[i].quantity,
               cart->items[i].unit_price,
               cart->items[i].total_price);
        subtotal += cart->items[i].total_price;
    }
    printf("%s+-----+-------------------------------+---------+---------+---------------+%s\n", COLOR_CYAN, COLOR_RESET);
    printf(" %48s: %sRs.%.2f%s\n", "Cart Subtotal", COLOR_BOLD, subtotal, COLOR_RESET);
}

void run_customer_portal() {
    clear_screen();
    print_banner();
    print_customer_banner();

    char student_id[16];
    char student_name[40] = "Guest Customer";
    float loyalty_balance = 0.0f;
    int is_student = 0;

    printf("\nEnter Student ID / Roll Number (or type 'GUEST'): ");
    if (scanf("%15s", student_id) != 1) return;

    int is_guest = 1;
    if (strlen(student_id) == 5) {
        if ((student_id[0] == 'g' || student_id[0] == 'G') &&
            (student_id[1] == 'u' || student_id[1] == 'U') &&
            (student_id[2] == 'e' || student_id[2] == 'E') &&
            (student_id[3] == 's' || student_id[3] == 'S') &&
            (student_id[4] == 't' || student_id[4] == 'T')) {
            is_guest = 1;
        } else {
            is_guest = 0;
        }
    } else {
        is_guest = 0;
    }

    if (!is_guest) {
        Student stu;
        if (find_student(student_id, &stu)) {
            is_student = 1;
            loyalty_balance = stu.loyalty_points;
            strncpy(student_name, stu.name, sizeof(student_name) - 1);
            printf("%s[Welcome back, %s! You have %.2f Loyalty Points (Rs.%.2f value)]%s\n",
                   COLOR_GREEN, student_name, loyalty_balance, loyalty_balance, COLOR_RESET);
        } else {
            is_student = 1;
            printf("Enter your Name for new student registration: ");
            scanf(" %[^\n]", student_name);
            printf("%s[New Student Account registered for %s with ID %s]%s\n",
                   COLOR_GREEN, student_name, student_id, COLOR_RESET);
        }
    } else {
        strcpy(student_id, "GUEST");
    }

    Cart *cart = init_cart();
    if (!cart) return;

    int choice;
    do {
        printf("\n%s================ CUSTOMER MENU ================%s\n", COLOR_CYAN, COLOR_RESET);
        printf("1. Browse Food Menu\n");
        printf("2. Add Item to Cart\n");
        printf("3. Remove Item from Cart\n");
        printf("4. View Cart (%d items)\n", cart->count);
        printf("5. Checkout & Generate Bill\n");
        printf("0. Cancel Order & Return to Main Menu\n");
        printf("%sChoose an option (0-5): %s", COLOR_BOLD, COLOR_RESET);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("%sInvalid input! Please enter a number.%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }

        switch (choice) {
            case 1: {
                clear_screen();
                print_customer_banner();
                Item items[100];
                int count = get_all_items(items, 100);
                print_menu_table(items, count, 0);
                pause_screen();
                break;
            }
            case 2: {
                Item items[100];
                int count = get_all_items(items, 100);
                print_menu_table(items, count, 0);

                int item_id, qty;
                printf("\nEnter Item ID to order: ");
                if (scanf("%d", &item_id) != 1) {
                    while (getchar() != '\n');
                    break;
                }

                Item selected;
                if (!find_item_by_id(item_id, &selected)) {
                    printf("%sItem ID %d not found or inactive!%s\n", COLOR_RED, item_id, COLOR_RESET);
                    break;
                }

                if (selected.stock == 0) {
                    printf("%sSorry, '%s' is completely SOLD OUT!%s\n", COLOR_RED, selected.name, COLOR_RESET);
                    break;
                }

                printf("Enter quantity (Available: %d): ", selected.stock);
                if (scanf("%d", &qty) != 1 || qty <= 0) {
                    while (getchar() != '\n');
                    printf("%sInvalid quantity!%s\n", COLOR_RED, COLOR_RESET);
                    break;
                }

                if (add_to_cart(cart, selected.id, selected.name, selected.price, qty, selected.stock)) {
                    printf("%s[Success] Added %dx %s to cart!%s\n", COLOR_GREEN, qty, selected.name, COLOR_RESET);
                }
                break;
            }
            case 3: {
                if (cart->count == 0) {
                    printf("%sYour cart is empty!%s\n", COLOR_YELLOW, COLOR_RESET);
                    break;
                }
                view_cart(cart);
                int rem_id;
                printf("Enter Item ID to remove from cart: ");
                if (scanf("%d", &rem_id) == 1) {
                    if (remove_from_cart(cart, rem_id)) {
                        printf("%s[Success] Item removed from cart.%s\n", COLOR_GREEN, COLOR_RESET);
                    } else {
                        printf("%sItem ID not found in cart.%s\n", COLOR_RED, COLOR_RESET);
                    }
                }
                break;
            }
            case 4: {
                view_cart(cart);
                pause_screen();
                break;
            }
            case 5: {
                if (cart->count == 0) {
                    printf("%sCannot checkout: Your cart is empty!%s\n", COLOR_RED, COLOR_RESET);
                    break;
                }

                clear_screen();
                print_customer_banner();
                view_cart(cart);

                float subtotal = 0.0f;
                for (int i = 0; i < cart->count; i++) {
                    subtotal += cart->items[i].total_price;
                }

                float discount = 0.0f;

                // Smart Combo Offer Suggestion (e.g. Sandwich + Chai or Dosa + Chai)
                int has_snack = 0, has_beverage = 0;
                for (int i = 0; i < cart->count; i++) {
                    Item itm;
                    if (find_item_by_id(cart->items[i].item_id, &itm)) {
                        if (itm.category == CAT_SNACKS || itm.category == CAT_MEALS) has_snack = 1;
                        if (itm.category == CAT_BEVERAGES) has_beverage = 1;
                    }
                }

                if (has_snack && has_beverage) {
                    printf("\n%s[Smart Combo Offer Applied!] Meal + Drink combo discount of Rs.10.00%s\n", COLOR_GREEN, COLOR_RESET);
                    discount += 10.0f;
                }

                // Student loyalty points redemption
                float points_used = 0.0f;
                if (is_student && loyalty_balance > 0.0f) {
                    printf("\nYou have %.2f Loyalty Points available (1 Point = Rs.1.00).\n", loyalty_balance);
                    printf("Do you want to redeem your points? (1 for Yes, 0 for No): ");
                    int redeem_choice = 0;
                    if (scanf("%d", &redeem_choice) == 1 && redeem_choice == 1) {
                        float max_allowed = subtotal - discount;
                        if (max_allowed < 0) max_allowed = 0;
                        points_used = (loyalty_balance > max_allowed) ? max_allowed : loyalty_balance;
                        discount += points_used;
                        printf("%sRedeemed %.2f points for Rs.%.2f discount!%s\n", COLOR_GREEN, points_used, points_used, COLOR_RESET);
                    }
                }

                float taxable_amount = subtotal - discount;
                if (taxable_amount < 0.0f) taxable_amount = 0.0f;
                float gst = taxable_amount * 0.05f; // 5% GST
                float grand_total = taxable_amount + gst;

                printf("\n%sBilling Summary:%s\n", COLOR_BOLD, COLOR_RESET);
                printf("  Subtotal    : Rs.%.2f\n", subtotal);
                printf("  Discount    : -Rs.%.2f\n", discount);
                printf("  GST (5%%)    : Rs.%.2f\n", gst);
                printf("  %sGRAND TOTAL : Rs.%.2f%s\n", COLOR_GREEN, grand_total, COLOR_RESET);

                // Select Payment Mode
                printf("\nSelect Payment Mode:\n");
                printf("1. UPI (GPay / PhonePe / Paytm)\n");
                printf("2. Cash\n");
                printf("3. Debit / Credit Card\n");
                printf("Choose payment mode (1-3): ");
                int p_choice = 1;
                scanf("%d", &p_choice);

                char payment_mode[12] = "CASH";
                if (p_choice == 1) strcpy(payment_mode, "UPI");
                else if (p_choice == 3) strcpy(payment_mode, "CARD");

                // Generate Order
                Order order;
                order.bill_number = get_next_bill_number();
                strncpy(order.student_id, student_id, sizeof(order.student_id) - 1);

                time_t now = time(NULL);
                strftime(order.date_time, sizeof(order.date_time), "%Y-%m-%d %H:%M", localtime(&now));

                order.item_count = (cart->count < 30) ? cart->count : 30;
                for (int i = 0; i < order.item_count; i++) {
                    order.items[i] = cart->items[i];
                }
                order.subtotal = subtotal;
                order.discount = discount;
                order.gst = gst;
                order.grand_total = grand_total;
                strncpy(order.payment_mode, payment_mode, sizeof(order.payment_mode) - 1);

                // Commit order to disk
                if (record_order(order)) {
                    // Deduct stock for all items
                    for (int i = 0; i < cart->count; i++) {
                        update_item_stock(cart->items[i].item_id, cart->items[i].quantity);
                    }

                    // Calculate 5% cashback loyalty points earned
                    float cashback_earned = grand_total * 0.05f;
                    if (is_student) {
                        update_student_points(student_id, student_name, cashback_earned, points_used);
                        printf("\n%s[Loyalty Update] You earned %.2f new points from this order!%s\n",
                               COLOR_GREEN, cashback_earned, COLOR_RESET);
                    }

                    // Print Receipt in terminal
                    print_receipt_terminal(order);

                    // Generate digital HTML receipt
                    export_web_receipt(order);
                    printf("%s[Digital E-Receipt saved to receipt_%d.html]%s\n", COLOR_CYAN, order.bill_number, COLOR_RESET);

                    pause_screen();
                    free_cart(cart);
                    return;
                } else {
                    printf("%s[Error] Failed to record order to sales.dat!%s\n", COLOR_RED, COLOR_RESET);
                }
                break;
            }
            case 0:
                printf("\nCancelling order and returning to main menu...\n");
                free_cart(cart);
                return;
            default:
                printf("%sInvalid choice! Please select 0 to 5.%s\n", COLOR_RED, COLOR_RESET);
        }
    } while (choice != 0);

    free_cart(cart);
}
