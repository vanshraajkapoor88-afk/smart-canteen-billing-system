#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/web_exporter.h"
#include "../include/file_manager.h"
#include "../include/colors.h"

// Category label helper
static const char* get_category_name(int cat) {
    switch (cat) {
        case CAT_BEVERAGES: return "Beverages";
        case CAT_SNACKS:    return "Snacks";
        case CAT_MEALS:     return "Meals";
        case CAT_DESSERTS:  return "Desserts";
        default:            return "General";
    }
}

void export_and_launch_web_dashboard() {
    Item items[100];
    int item_count = get_all_items(items, 100);

    Order orders[200];
    int order_count = get_all_orders(orders, 200);

    // Compute metrics
    float total_revenue = 0.0f;
    int low_stock_count = 0;
    int sold_out_count = 0;

    for (int i = 0; i < item_count; i++) {
        if (!items[i].is_active) continue;
        if (items[i].stock == 0) sold_out_count++;
        else if (items[i].stock < 5) low_stock_count++;
    }

    // Find best-selling item
    typedef struct {
        int id;
        char name[40];
        int qty_sold;
        float revenue;
    } ItemSale;

    ItemSale sales_stats[100];
    int stats_count = 0;

    for (int o = 0; o < order_count; o++) {
        total_revenue += orders[o].grand_total;
        for (int it = 0; it < orders[o].item_count; it++) {
            CartItem ci = orders[o].items[it];
            int found = 0;
            for (int s = 0; s < stats_count; s++) {
                if (sales_stats[s].id == ci.item_id) {
                    sales_stats[s].qty_sold += ci.quantity;
                    sales_stats[s].revenue += ci.total_price;
                    found = 1;
                    break;
                }
            }
            if (!found && stats_count < 100) {
                sales_stats[stats_count].id = ci.item_id;
                strncpy(sales_stats[stats_count].name, ci.name, 39);
                sales_stats[stats_count].qty_sold = ci.quantity;
                sales_stats[stats_count].revenue = ci.total_price;
                stats_count++;
            }
        }
    }

    char best_seller_name[40] = "None yet";
    int best_seller_qty = 0;
    for (int s = 0; s < stats_count; s++) {
        if (sales_stats[s].qty_sold > best_seller_qty) {
            best_seller_qty = sales_stats[s].qty_sold;
            strncpy(best_seller_name, sales_stats[s].name, 39);
        }
    }

    // Open file for writing dashboard
    FILE *fp = fopen("dashboard.html", "w");
    if (!fp) {
        printf("%s[Error] Could not generate dashboard.html%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    time_t now = time(NULL);
    char time_str[30];
    strftime(time_str, sizeof(time_str), "%d %b %Y, %I:%M %p", localtime(&now));

    // Write HTML content with sleek modern CSS
    fprintf(fp, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    fprintf(fp, "<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fp, "<title>Smart Canteen - Live POS Dashboard</title>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "* { margin:0; padding:0; box-sizing:border-box; font-family:'Segoe UI',Roboto,Helvetica,sans-serif; }\n");
    fprintf(fp, "body { background:#0f172a; color:#f8fafc; padding:24px; }\n");
    fprintf(fp, ".container { max-width:1200px; margin:0 auto; }\n");
    fprintf(fp, ".header { display:flex; justify-content:space-between; align-items:center; padding-bottom:20px; border-bottom:1px solid #334155; margin-bottom:24px; }\n");
    fprintf(fp, ".header h1 { font-size:1.8rem; font-weight:700; background:linear-gradient(90deg, #38bdf8, #818cf8); -webkit-background-clip:text; -webkit-text-fill-color:transparent; }\n");
    fprintf(fp, ".header .time { color:#94a3b8; font-size:0.9rem; }\n");
    fprintf(fp, ".status-badge { display:inline-flex; align-items:center; gap:6px; background:#1e293b; color:#10b981; padding:6px 14px; border-radius:20px; font-size:0.85rem; font-weight:600; border:1px solid #059669; }\n");
    fprintf(fp, ".status-dot { width:8px; height:8px; background:#10b981; border-radius:50%; box-shadow:0 0 8px #10b981; }\n");
    fprintf(fp, ".grid-kpi { display:grid; grid-template-columns:repeat(auto-fit, minmax(240px, 1fr)); gap:18px; margin-bottom:30px; }\n");
    fprintf(fp, ".card { background:#1e293b; border:1px solid #334155; border-radius:14px; padding:20px; box-shadow:0 10px 15px -3px rgba(0,0,0,0.3); }\n");
    fprintf(fp, ".card .title { font-size:0.85rem; text-transform:uppercase; color:#94a3b8; letter-spacing:0.05em; margin-bottom:8px; }\n");
    fprintf(fp, ".card .value { font-size:1.9rem; font-weight:800; color:#ffffff; }\n");
    fprintf(fp, ".card .sub { font-size:0.8rem; color:#64748b; margin-top:6px; }\n");
    fprintf(fp, ".kpi-green { border-left:4px solid #10b981; }\n");
    fprintf(fp, ".kpi-blue { border-left:4px solid #38bdf8; }\n");
    fprintf(fp, ".kpi-yellow { border-left:4px solid #f59e0b; }\n");
    fprintf(fp, ".kpi-purple { border-left:4px solid #a855f7; }\n");
    fprintf(fp, ".section { margin-bottom:32px; }\n");
    fprintf(fp, ".section h2 { font-size:1.3rem; margin-bottom:16px; color:#e2e8f0; display:flex; align-items:center; justify-content:space-between; }\n");
    fprintf(fp, "table { width:100%; border-collapse:collapse; background:#1e293b; border-radius:12px; overflow:hidden; border:1px solid #334155; }\n");
    fprintf(fp, "th { background:#0f172a; color:#94a3b8; font-weight:600; text-align:left; padding:12px 16px; font-size:0.85rem; border-bottom:1px solid #334155; }\n");
    fprintf(fp, "td { padding:14px 16px; border-bottom:1px solid #273549; font-size:0.9rem; }\n");
    fprintf(fp, "tr:hover { background:#253347; }\n");
    fprintf(fp, ".badge { padding:4px 10px; border-radius:12px; font-size:0.75rem; font-weight:700; display:inline-block; }\n");
    fprintf(fp, ".badge-in { background:rgba(16,185,129,0.15); color:#34d399; border:1px solid rgba(16,185,129,0.4); }\n");
    fprintf(fp, ".badge-low { background:rgba(245,158,11,0.15); color:#fbbf24; border:1px solid rgba(245,158,11,0.4); }\n");
    fprintf(fp, ".badge-out { background:rgba(239,68,68,0.15); color:#f87171; border:1px solid rgba(239,68,68,0.4); }\n");
    fprintf(fp, ".btn-print { background:#3b82f6; color:#fff; border:none; padding:8px 16px; border-radius:8px; cursor:pointer; font-weight:600; font-size:0.85rem; transition:0.2s; }\n");
    fprintf(fp, ".btn-print:hover { background:#2563eb; }\n");
    fprintf(fp, ".footer { text-align:center; color:#64748b; font-size:0.85rem; margin-top:40px; padding-top:20px; border-top:1px solid #334155; }\n");
    fprintf(fp, "</style>\n</head>\n<body>\n");

    fprintf(fp, "<div class=\"container\">\n");
    fprintf(fp, "  <div class=\"header\">\n");
    fprintf(fp, "    <div>\n");
    fprintf(fp, "      <h1>Smart Canteen POS Dashboard</h1>\n");
    fprintf(fp, "      <div class=\"time\">Auto-generated by C Billing Core &bull; %s</div>\n", time_str);
    fprintf(fp, "    </div>\n");
    fprintf(fp, "    <div class=\"status-badge\"><div class=\"status-dot\"></div> C System Online</div>\n");
    fprintf(fp, "  </div>\n");

    // KPI Cards
    fprintf(fp, "  <div class=\"grid-kpi\">\n");
    fprintf(fp, "    <div class=\"card kpi-green\">\n");
    fprintf(fp, "      <div class=\"title\">Total Revenue</div>\n");
    fprintf(fp, "      <div class=\"value\">&#8377;%.2f</div>\n", total_revenue);
    fprintf(fp, "      <div class=\"sub\">From %d completed orders</div>\n", order_count);
    fprintf(fp, "    </div>\n");

    fprintf(fp, "    <div class=\"card kpi-blue\">\n");
    fprintf(fp, "      <div class=\"title\">Total Orders</div>\n");
    fprintf(fp, "      <div class=\"value\">%d</div>\n", order_count);
    fprintf(fp, "      <div class=\"sub\">Stored in data/sales.dat</div>\n");
    fprintf(fp, "    </div>\n");

    fprintf(fp, "    <div class=\"card kpi-purple\">\n");
    fprintf(fp, "      <div class=\"title\">Best-Selling Dish</div>\n");
    fprintf(fp, "      <div class=\"value\" style=\"font-size:1.4rem;\">%s</div>\n", best_seller_name);
    fprintf(fp, "      <div class=\"sub\">%d units sold today</div>\n", best_seller_qty);
    fprintf(fp, "    </div>\n");

    fprintf(fp, "    <div class=\"card kpi-yellow\">\n");
    fprintf(fp, "      <div class=\"title\">Inventory Alerts</div>\n");
    fprintf(fp, "      <div class=\"value\">%d Low / %d Out</div>\n", low_stock_count, sold_out_count);
    fprintf(fp, "      <div class=\"sub\">Requires restock attention</div>\n");
    fprintf(fp, "    </div>\n");
    fprintf(fp, "  </div>\n");

    // Inventory Table
    fprintf(fp, "  <div class=\"section\">\n");
    fprintf(fp, "    <h2><span>Live Food Inventory & Stock Status</span></h2>\n");
    fprintf(fp, "    <table>\n");
    fprintf(fp, "      <thead>\n");
    fprintf(fp, "        <tr><th style=\"width:70px;\">S.No</th><th>Item Name</th><th>Category</th><th>Price (INR)</th><th>Stock Remaining</th><th>Status</th></tr>\n");
    fprintf(fp, "      </thead>\n      <tbody>\n");

    int sno = 1;
    for (int i = 0; i < item_count; i++) {
        if (!items[i].is_active) continue;
        const char *badge_class = "badge-in";
        const char *badge_text = "IN STOCK";

        if (items[i].stock == 0) {
            badge_class = "badge-out";
            badge_text = "SOLD OUT";
        } else if (items[i].stock < 5) {
            badge_class = "badge-low";
            badge_text = "LOW STOCK";
        }

        fprintf(fp, "        <tr>\n");
        fprintf(fp, "          <td><strong style=\"color:#38bdf8;\">%d</strong></td>\n", sno++);
        fprintf(fp, "          <td><strong>%s</strong></td>\n", items[i].name);
        fprintf(fp, "          <td>%s</td>\n", get_category_name(items[i].category));
        fprintf(fp, "          <td>&#8377;%.2f</td>\n", items[i].price);
        fprintf(fp, "          <td>%d units</td>\n", items[i].stock);
        fprintf(fp, "          <td><span class=\"badge %s\">%s</span></td>\n", badge_class, badge_text);
        fprintf(fp, "        </tr>\n");
    }
    fprintf(fp, "      </tbody>\n    </table>\n");
    fprintf(fp, "  </div>\n");

    // Recent Sales Orders Table
    fprintf(fp, "  <div class=\"section\">\n");
    fprintf(fp, "    <h2><span>Recent Sales Orders & Receipts</span> <button class=\"btn-print\" onclick=\"window.print()\">Print Full Report</button></h2>\n");
    fprintf(fp, "    <table>\n");
    fprintf(fp, "      <thead>\n");
    fprintf(fp, "        <tr><th>Bill #</th><th>Date / Time</th><th>Customer</th><th>Items</th><th>Total</th><th>Payment</th></tr>\n");
    fprintf(fp, "      </thead>\n      <tbody>\n");

    int start_order = (order_count > 10) ? order_count - 10 : 0;
    for (int o = order_count - 1; o >= start_order; o--) {
        char item_summary[128] = "";
        for (int it = 0; it < orders[o].item_count && it < 3; it++) {
            char part[40];
            snprintf(part, sizeof(part), "%dx %s%s", orders[o].items[it].quantity, orders[o].items[it].name, (it == orders[o].item_count - 1 || it == 2) ? "" : ", ");
            strcat(item_summary, part);
        }
        if (orders[o].item_count > 3) strcat(item_summary, "...");

        fprintf(fp, "        <tr>\n");
        fprintf(fp, "          <td><strong>#%d</strong></td>\n", orders[o].bill_number);
        fprintf(fp, "          <td>%s</td>\n", orders[o].date_time);
        fprintf(fp, "          <td><code>%s</code></td>\n", orders[o].student_id);
        fprintf(fp, "          <td>%s</td>\n", item_summary);
        fprintf(fp, "          <td><strong>&#8377;%.2f</strong></td>\n", orders[o].grand_total);
        fprintf(fp, "          <td><span class=\"badge badge-in\">%s</span></td>\n", orders[o].payment_mode);
        fprintf(fp, "        </tr>\n");
    }

    if (order_count == 0) {
        fprintf(fp, "        <tr><td colspan=\"6\" style=\"text-align:center; color:#64748b;\">No orders recorded yet.</td></tr>\n");
    }

    fprintf(fp, "      </tbody>\n    </table>\n");
    fprintf(fp, "  </div>\n");

    fprintf(fp, "  <div class=\"footer\">\n");
    fprintf(fp, "    Built in Pure C &bull; Smart Canteen Billing & POS System &bull; B.Tech CSE Project\n");
    fprintf(fp, "  </div>\n");
    fprintf(fp, "</div>\n</body>\n</html>\n");

    fclose(fp);

    printf("%s[Success] Web Dashboard compiled to dashboard.html%s\n", COLOR_GREEN, COLOR_RESET);
#ifdef _WIN32
    system("start dashboard.html");
#else
    system("xdg-open dashboard.html || open dashboard.html");
#endif
}

void export_web_receipt(Order order) {
    char filename[64];
    snprintf(filename, sizeof(filename), "receipt_%d.html", order.bill_number);
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp, "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n");
    fprintf(fp, "<title>Receipt #%d - Smart Canteen</title>\n", order.bill_number);
    fprintf(fp, "<style>\n");
    fprintf(fp, "body { font-family: 'Courier New', monospace; background:#f4f4f9; padding:20px; display:flex; justify-content:center; }\n");
    fprintf(fp, ".receipt { background:#fff; padding:24px; border:1px solid #ccc; width:340px; box-shadow:0 4px 10px rgba(0,0,0,0.1); border-radius:8px; }\n");
    fprintf(fp, ".center { text-align:center; }\n");
    fprintf(fp, "hr { border-top:1px dashed #666; margin:12px 0; }\n");
    fprintf(fp, ".row { display:flex; justify-content:space-between; margin:4px 0; font-size:13px; }\n");
    fprintf(fp, ".bold { font-weight:bold; }\n");
    fprintf(fp, ".btn { display:block; width:100%%; margin-top:16px; padding:10px; background:#10b981; color:#fff; border:none; border-radius:4px; font-weight:bold; cursor:pointer; text-align:center; text-decoration:none; }\n");
    fprintf(fp, "@media print { .btn { display:none; } body { background:#fff; padding:0; } }\n");
    fprintf(fp, "</style>\n</head>\n<body>\n");
    fprintf(fp, "<div class=\"receipt\">\n");
    fprintf(fp, "  <div class=\"center bold\" style=\"font-size:16px;\">CAMPUS SMART CANTEEN</div>\n");
    fprintf(fp, "  <div class=\"center\" style=\"font-size:12px;\">Tax Invoice / E-Receipt</div>\n");
    fprintf(fp, "  <hr>\n");
    fprintf(fp, "  <div class=\"row\"><span>Bill No:</span><span class=\"bold\">#%d</span></div>\n", order.bill_number);
    fprintf(fp, "  <div class=\"row\"><span>Date/Time:</span><span>%s</span></div>\n", order.date_time);
    fprintf(fp, "  <div class=\"row\"><span>Customer:</span><span>%s</span></div>\n", order.student_id);
    fprintf(fp, "  <div class=\"row\"><span>Payment:</span><span>%s</span></div>\n", order.payment_mode);
    fprintf(fp, "  <hr>\n");

    for (int i = 0; i < order.item_count; i++) {
        fprintf(fp, "  <div class=\"row\"><span>%s (x%d)</span><span>&#8377;%.2f</span></div>\n",
                order.items[i].name, order.items[i].quantity, order.items[i].total_price);
    }

    fprintf(fp, "  <hr>\n");
    fprintf(fp, "  <div class=\"row\"><span>Subtotal:</span><span>&#8377;%.2f</span></div>\n", order.subtotal);
    if (order.discount > 0.0f) {
        fprintf(fp, "  <div class=\"row\" style=\"color:#059669;\"><span>Discount:</span><span>-&#8377;%.2f</span></div>\n", order.discount);
    }
    fprintf(fp, "  <div class=\"row\"><span>GST (5%%):</span><span>&#8377;%.2f</span></div>\n", order.gst);
    fprintf(fp, "  <hr>\n");
    fprintf(fp, "  <div class=\"row bold\" style=\"font-size:15px;\"><span>GRAND TOTAL:</span><span>&#8377;%.2f</span></div>\n", order.grand_total);
    fprintf(fp, "  <hr>\n");
    fprintf(fp, "  <div class=\"center\" style=\"font-size:11px;\">Thank you! Visit again.<br>E-Receipt powered by C Billing Engine</div>\n");
    fprintf(fp, "  <button class=\"btn\" onclick=\"window.print()\">Print Receipt</button>\n");
    fprintf(fp, "</div>\n</body>\n</html>\n");
    fclose(fp);
}
