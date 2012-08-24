#ifndef _KRAKEN_EXPORT_H
#define _KRAKEN_EXPORT_H

#include "kraken.h"

int export_host_manager_to_csv(host_manager *c_host_manager, const char *dest_file);
int export_host_manager_to_xml(host_manager *c_host_manager, const char *dest_file);
int import_host_manager_from_xml(host_manager *c_host_manager, const char *source_file);

#endif