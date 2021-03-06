// gui_popups_threads.h
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of SecureState Consulting nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _KRAKEN_GUI_POPUPS_THREADS_H
#define _KRAKEN_GUI_POPUPS_THREADS_H

#include "kraken.h"
#include "dns_enum.h"
#include "gui_popups.h"
#include "http_scan.h"
#include "network_addr.h"

void callback_thread_start(GtkWidget *widget, popup_data *p_data);
void callback_thread_cancel_action(GtkWidget *widget, popup_data *p_data);
void callback_thread_update_progress(unsigned int current, unsigned int high, popup_data *p_data);

void gui_popup_thread_dns_enum_domain(popup_data *p_data);
void gui_popup_thread_dns_enum_network(popup_data *gpt_data);
void gui_popup_thread_http_scrape_url_for_links(popup_data *p_data);
void gui_popup_thread_http_scrape_hosts_for_links(popup_data *p_data);
void gui_popup_thread_http_search_engine_bing_domain(popup_data *p_data);
void gui_popup_thread_http_search_engine_bing_ip(popup_data *p_data);
void gui_popup_thread_http_search_engine_bing_all_ips(popup_data *p_data);
void gui_popup_thread_import_file(popup_data *p_data);

#endif
