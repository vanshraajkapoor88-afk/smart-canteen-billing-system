#ifndef WEB_EXPORTER_H
#define WEB_EXPORTER_H

#include "models.h"

// Generates dashboard.html from current binary records and opens it in default web browser
void export_and_launch_web_dashboard();

// Generates an individual standalone HTML E-Receipt for an order
void export_web_receipt(Order order);

#endif // WEB_EXPORTER_H
