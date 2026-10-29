# 🍱 Smart Canteen POS & Billing System

A terminal-driven Point of Sale (POS) and inventory management system built completely in **pure C (Standard C99)** with an automated, responsive **HTML5/CSS3 Web POS Dashboard generator**.

Developed as a 1st-Year B.Tech CSE Hackathon project demonstrating computer science fundamentals: dynamic memory allocation, pointer manipulation, binary file persistence, and modular software architecture.

---

## 🌟 Key Features

### 🖥️ Customer Terminal Portal
- **ANSI Color Terminal UI**: Clear ASCII art banners and colored status badges (`IN STOCK`, `LOW STOCK`, `SOLD OUT`).
- **Dynamic Cart Memory**: Cart expands dynamically in heap memory via `malloc()` and `realloc()`, and frees via `free()`.
- **Student Loyalty Engine**: 5% automatic cashback credited to Student Roll Numbers (`students.dat`) with 1-click points redemption.
- **Smart Combo Offers**: Auto-applies discounts when bundling snacks and drinks.
- **Detailed Invoicing**: Prints GST-compliant receipts with invoice numbers, itemized rate breakdown, taxes, and discounts.

### 🛡️ Admin & Stock Control Panel (Password Protected)
- **Inventory CRUD**: Add, update prices/quantities, and soft-delete items without breaking historical billing data.
- **Low-Stock Shields**: Real-time warnings when items fall below 5 units, and automatic order-blocking when sold out.
- **Sales Analytics**: Aggregates gross revenue, GST collected, total orders, and best-selling dishes of the day.

### 🌐 Smart Web Dashboard & E-Receipts (The X-Factor)
- At the touch of a button, C reads `sales.dat` and `menu.dat` using `fread()` and writes out a sleek, modern **HTML5 Web Dashboard** (`dashboard.html`) and digital web receipts (`receipt_<id>.html`), automatically launching them in the default web browser.

---

## 📂 Project Architecture

```text
billing_system/
├── include/                 # Header files (declarations & contracts)
│   ├── models.h             # Item, Cart, Order, and Student structs
│   ├── colors.h             # ANSI colors & Windows console mode
│   ├── file_manager.h       # Binary file CRUD operations
│   ├── ui_utils.h           # Visual formatting, banners, tables
│   ├── customer.h           # Customer order workflow & dynamic cart
│   ├── admin.h              # Admin control panel
│   └── web_exporter.h       # HTML dashboard & e-receipt generator
├── src/                     # C Implementation files
│   ├── main.c               # Program entry point & main routing loop
│   ├── file_manager.c       # Binary file I/O (fread, fwrite, fseek)
│   ├── ui_utils.c           # Terminal UI rendering & receipt printing
│   ├── customer.c           # Dynamic cart (malloc/realloc/free)
│   ├── admin.c              # Inventory CRUD & sales analytics
│   └── web_exporter.c       # Web dashboard HTML/CSS compiler
├── data/                    # Persistent storage (binary data)
│   ├── menu.dat             # Food inventory
│   ├── sales.dat            # Sales transactions
│   └── students.dat         # Student loyalty balances
├── compile.bat              # Windows GCC compilation script
├── RUN.bat                  # 1-Click quick launch script
├── VIEW_WEB_DASHBOARD.bat   # 1-Click web dashboard viewer
└── HOW_TO_RUN.txt           # Credentials & quick start instructions
```

---

## 🚀 How to Run

### Windows (Easiest)
Simply double-click **`RUN.bat`**. It will automatically compile the code with GCC if needed and launch the console application.

To manually compile using GCC:
```bash
gcc -Wall -Wextra -std=c99 src/main.c src/file_manager.c src/ui_utils.c src/customer.c src/admin.c src/web_exporter.c -Iinclude -o canteen.exe
./canteen.exe
```

### Linux / macOS
```bash
gcc -Wall -Wextra -std=c99 src/*.c -Iinclude -o canteen
./canteen
```

---

## 🔑 Demo Credentials

| Role | Details |
| :--- | :--- |
| **Admin Password** | `admin123` |
| **Pre-registered Students** | `2026CSE001` (Aarav), `2026CSE042` (Sneha), `2026CSE108` (Rohan) |
| **Guest Customers** | Type `GUEST` during customer checkout |

---

## 📜 License
Open-source under the MIT License. Built for educational and hackathon demonstration purposes.
