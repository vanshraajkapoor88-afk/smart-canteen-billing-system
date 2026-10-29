#ifndef MODELS_H
#define MODELS_H

// Category definitions for food items
typedef enum {
    CAT_BEVERAGES = 1,
    CAT_SNACKS,
    CAT_MEALS,
    CAT_DESSERTS
} Category;

// Food item structure stored in data/menu.dat
typedef struct {
    int id;                  // Unique item ID (e.g., 101)
    char name[40];           // Item name (e.g., "Masala Dosa")
    float price;             // Price per unit (in Rupees)
    int stock;               // Available quantity
    int category;            // Category (1 to 4)
    int is_active;           // 1 = active, 0 = soft-deleted
} Item;

// Individual item line in customer cart
typedef struct {
    int item_id;
    char name[40];
    float unit_price;
    int quantity;
    float total_price;       // quantity * unit_price
} CartItem;

// Dynamic Cart (uses malloc / realloc / free)
typedef struct {
    CartItem *items;         // Dynamic array pointer
    int count;               // Current number of items in cart
    int capacity;            // Current memory capacity allocated
} Cart;

// Student loyalty profile stored in data/students.dat
typedef struct {
    char student_id[16];     // Roll number / ID (e.g., "2026CSE0142")
    char name[40];           // Student name
    float loyalty_points;    // Earned points (1 point = ₹1 discount)
} Student;

// Completed order record stored in data/sales.dat
typedef struct {
    int bill_number;         // Unique auto-incremented bill ID
    char student_id[16];     // Student ID or "GUEST"
    char date_time[26];      // Timestamp (e.g., "2026-10-29 19:30")
    int item_count;          // Number of distinct items
    CartItem items[30];      // Fixed snapshot array for file storage
    float subtotal;          // Items total before discounts & taxes
    float discount;          // Discount from loyalty points / coupons
    float gst;               // 5% GST
    float grand_total;       // Final amount paid
    char payment_mode[12];   // "CASH", "UPI", or "CARD"
} Order;

#endif // MODELS_H
