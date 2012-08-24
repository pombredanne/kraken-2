#include "kraken.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "dns_enum.h"
#include "gui_popups.h"
#include "gui_popups_threads.h"
#include "gui_model.h"
#include "host_manager.h"
#include "http_scan.h"
#include "network_addr.h"
#include "whois_lookup.h"

void callback_thread_start(GtkWidget *widget, popup_data *p_data) {
	kraken_thread k_thread;

	kraken_thread_create(&k_thread, p_data->thread_function, p_data);
	return;
}

void callback_thread_cancel_action(GtkWidget *widget, popup_data *p_data) {
	GtkWidget *dialog;

	if (p_data->action_status != KRAKEN_ACTION_RUN) {
		return;
	}
	if (p_data->cancel_button != NULL) {
		gtk_widget_set_sensitive(p_data->cancel_button, FALSE);
	}
	dialog = gtk_message_dialog_new(GTK_WINDOW(p_data->popup_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, 0, "Please Wait");
	gtk_window_set_title(GTK_WINDOW(dialog), "Please Wait");
	p_data->cancel_dialog = dialog;
	p_data->action_status = KRAKEN_ACTION_STOP;
	gtk_dialog_run(GTK_DIALOG(dialog));
	return;
}

gboolean callback_thread_window_destroy(GtkWidget *widget, void *bullshit, popup_data *p_data) {
	callback_thread_cancel_action(widget, p_data);
	return TRUE;
}

void callback_thread_update_progress(unsigned int current, unsigned int high, popup_data *p_data) {
	gdouble percent = (gdouble)current / high;
	if (GTK_IS_PROGRESS_BAR(p_data->misc_widget)) {
		gdk_threads_enter();
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(p_data->misc_widget), percent);
		gdk_threads_leave();
	}
	return;
}

void gui_popup_thread_dns_enum_domain(popup_data *p_data) {
	dns_enum_opts d_opts;
	const gchar *text_entry;
	int response;
	char target_domain[DNS_MAX_FQDN_LENGTH + 1];
	gulong delete_handler;

	memset(target_domain, '\0', sizeof(target_domain));
	gdk_threads_enter();
	text_entry = gtk_entry_get_text(GTK_ENTRY(p_data->text_entry0));
	if ((strlen(text_entry) > DNS_MAX_FQDN_LENGTH) || (strlen(text_entry) == 0)) {
		GUI_POPUP_ERROR_INVALID_DOMAIN_NAME(p_data->popup_window);
		gtk_widget_destroy(p_data->popup_window);
		return;
	}
	strncpy(target_domain, text_entry, DNS_MAX_FQDN_LENGTH);

	gtk_widget_set_sensitive(p_data->cancel_button, TRUE);
	gtk_widget_set_sensitive(p_data->start_button, FALSE);
	delete_handler = g_signal_connect(p_data->popup_window, "delete-event", G_CALLBACK(callback_thread_window_destroy), p_data);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(p_data->misc_widget), "Enumerating Domain");
	gui_model_update_marquee((main_gui_data *)p_data, "Enumerating Domain");
	gdk_threads_leave();

	dns_enum_opts_init(&d_opts);
	dns_enum_opts_set_wordlist(&d_opts, p_data->k_opts->dns_wordlist);
	d_opts.progress_update = (void *)&callback_thread_update_progress;
	d_opts.progress_update_data = p_data;
	d_opts.action_status = &p_data->action_status;
	p_data->action_status = KRAKEN_ACTION_RUN;

	response = dns_enum_domain_ex(p_data->c_host_manager, target_domain, &d_opts);

	gdk_threads_enter();
	if (p_data->action_status == KRAKEN_ACTION_STOP) {
		if (p_data->cancel_dialog != NULL) {
			gtk_widget_destroy(p_data->cancel_dialog);
		}
		p_data->action_status = KRAKEN_ACTION_RUN;
	}
	if (response == -1) {
		GUI_POPUP_ERROR_INVALID_DOMAIN_NAME(p_data->popup_window);
		gui_model_update_marquee((main_gui_data *)p_data, NULL);
	} else {
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(p_data->misc_widget), "Requesting WHOIS Records");
		gui_model_update_marquee((main_gui_data *)p_data, "Requesting WHOIS Records");
		gdk_threads_leave();
		whois_fill_host_manager(p_data->c_host_manager);
		gdk_threads_enter();
		gui_model_update_tree_and_marquee((main_gui_data *)p_data, NULL);
	}
	g_signal_handler_disconnect(p_data->popup_window, delete_handler);
	gtk_widget_destroy(p_data->popup_window);
	gdk_threads_leave();
	dns_enum_opts_destroy(&d_opts);
	return;
}

void gui_popup_thread_dns_enum_network(popup_data *p_data) {
	dns_enum_opts d_opts;
	const gchar *text_entry;
	int response;
	char target_domain[DNS_MAX_FQDN_LENGTH + 1];
	network_addr target_network;
	gulong delete_handler;

	memset(target_domain, '\0', sizeof(target_domain));
	gdk_threads_enter();
	text_entry = gtk_entry_get_text(GTK_ENTRY(p_data->text_entry0));
	if ((strlen(text_entry) > DNS_MAX_FQDN_LENGTH) || (strlen(text_entry) == 0)) {
		GUI_POPUP_ERROR_INVALID_DOMAIN_NAME(p_data->popup_window);
		gtk_widget_destroy(p_data->popup_window);
		gdk_threads_leave();
		return;
	}
	strncpy(target_domain, text_entry, DNS_MAX_FQDN_LENGTH);

	text_entry = gtk_entry_get_text(GTK_ENTRY(p_data->text_entry1));
	if (netaddr_cidr_str_to_nwk(&target_network, (char *)text_entry) != 0) {
		GUI_POPUP_ERROR_INVALID_CIDR_NETWORK(p_data->popup_window);
		gtk_widget_destroy(p_data->popup_window);
		gdk_threads_leave();
		return;
	}
	gtk_widget_set_sensitive(p_data->cancel_button, TRUE);
	gtk_widget_set_sensitive(p_data->start_button, FALSE);
	delete_handler = g_signal_connect(p_data->popup_window, "delete-event", G_CALLBACK(callback_thread_window_destroy), p_data);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(p_data->misc_widget), "Enumerating Network");
	gui_model_update_marquee((main_gui_data *)p_data, "Enumerating Network");
	gdk_threads_leave();

	dns_enum_opts_init(&d_opts);
	d_opts.progress_update = (void *)&callback_thread_update_progress;
	d_opts.progress_update_data = p_data;
	d_opts.action_status = &p_data->action_status;
	p_data->action_status = KRAKEN_ACTION_RUN;

	response = dns_enum_network_ex(p_data->c_host_manager, target_domain, &target_network, &d_opts);

	gdk_threads_enter();
	if (response == 0) {
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(p_data->misc_widget), "Requesting WHOIS Records");
		gui_model_update_marquee((main_gui_data *)p_data, "Requesting WHOIS Records");
		gdk_threads_leave();
		whois_fill_host_manager(p_data->c_host_manager);
		gdk_threads_enter();
		gui_model_update_tree_and_marquee((main_gui_data*)p_data, NULL);
	}
	g_signal_handler_disconnect(p_data->popup_window, delete_handler);
	gtk_widget_destroy(p_data->popup_window);
	gdk_threads_leave();
	dns_enum_opts_destroy(&d_opts);
	return;
}

void gui_popup_thread_http_scrape_url_for_links(popup_data *p_data) {
	const gchar *text_entry;
	http_link *link_anchor = NULL;
	int tmp_val = 0;
	char target_host[DNS_MAX_FQDN_LENGTH + 1];
	char target_url[DNS_MAX_FQDN_LENGTH + 10];
	gulong delete_handler;

	memset(target_host, '\0', sizeof(target_host));
	gdk_threads_enter();
	text_entry = gtk_entry_get_text(GTK_ENTRY(p_data->text_entry0));
	if ((strlen(text_entry) > DNS_MAX_FQDN_LENGTH) || (strlen(text_entry) == 0)) {
		GUI_POPUP_ERROR_INVALID_HOST_NAME(p_data->popup_window);
		gtk_widget_destroy(p_data->popup_window);
		gdk_threads_leave();
		return;
	}
	delete_handler = g_signal_connect(p_data->popup_window, "delete-event", G_CALLBACK(callback_thread_window_destroy), p_data);
	gdk_threads_leave();

	strncpy(target_host, text_entry, DNS_MAX_FQDN_LENGTH);
	for (tmp_val = 0; tmp_val < strlen(target_host); tmp_val++) {
		if (*(char *)&target_host[tmp_val] == '/') {
			target_host[tmp_val] = '\0';
			break;
		}
	}
	snprintf(target_url, DNS_MAX_FQDN_LENGTH + 10, "http://%s/", target_host);
	tmp_val = http_scrape_url_for_links(target_url, &link_anchor);
	if (tmp_val) {
		LOGGING_QUICK_ERROR("kraken.gui.popup", "there was an error requesting the page")
	}
	gdk_threads_enter();
	gui_popup_select_hosts_from_http_links((main_gui_data *)p_data, link_anchor);
	g_signal_handler_disconnect(p_data->popup_window, delete_handler);
	gtk_widget_destroy(p_data->popup_window);
	gdk_threads_leave();
	http_free_link(link_anchor);
	return;
}

void gui_popup_thread_http_scrape_hosts_for_links(popup_data *p_data) {
	http_link *link_anchor = NULL;
	kraken_thread k_thread;
	http_enum_opts h_opts;
	gulong delete_handler;

	gdk_threads_enter();
	gtk_widget_set_sensitive(p_data->cancel_button, TRUE);
	gtk_widget_set_sensitive(p_data->start_button, FALSE);
	delete_handler = g_signal_connect(p_data->popup_window, "delete-event", G_CALLBACK(callback_thread_window_destroy), p_data);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(p_data->misc_widget), "Scanning For Links");
	gui_model_update_marquee((main_gui_data *)p_data, "Scanning For Links");
	gdk_threads_leave();

	http_enum_opts_init(&h_opts);
	h_opts.progress_update = (void *)&callback_thread_update_progress;
	h_opts.progress_update_data = p_data;
	h_opts.action_status = &p_data->action_status;
	p_data->action_status = KRAKEN_ACTION_RUN;

	http_scrape_hosts_for_links_ex(p_data->c_host_manager, &link_anchor, &h_opts);

	gdk_threads_enter();
	gui_popup_select_hosts_from_http_links((main_gui_data *)p_data, link_anchor);
	g_signal_handler_disconnect(p_data->popup_window, delete_handler);
	gtk_widget_destroy(p_data->popup_window);
	gdk_threads_leave();
	http_enum_opts_destroy(&h_opts);
	return;
}

void gui_popup_thread_http_search_engine_bing(popup_data *p_data) {
	http_enum_opts h_opts;
	const gchar *text_entry;
	int response;
	char target_domain[DNS_MAX_FQDN_LENGTH + 1];
	gulong delete_handler;

	memset(target_domain, '\0', sizeof(target_domain));
	gdk_threads_enter();
	text_entry = gtk_entry_get_text(GTK_ENTRY(p_data->text_entry0));
	gdk_threads_leave();
	if ((strlen(text_entry) > DNS_MAX_FQDN_LENGTH) || (strlen(text_entry) == 0)) {
		gdk_threads_enter();
		GUI_POPUP_ERROR_INVALID_DOMAIN_NAME(p_data->popup_window);
		gtk_widget_destroy(p_data->popup_window);
		gdk_threads_leave();
		return;
	}
	strncpy(target_domain, text_entry, DNS_MAX_FQDN_LENGTH);

	gdk_threads_enter();
	gtk_widget_set_sensitive(p_data->cancel_button, TRUE);
	gtk_widget_set_sensitive(p_data->start_button, FALSE);
	delete_handler = g_signal_connect(p_data->popup_window, "delete-event", G_CALLBACK(callback_thread_window_destroy), p_data);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(p_data->misc_widget), "Searching Bing");
	gui_model_update_marquee((main_gui_data *)p_data, "Searching Bing");
	gdk_threads_leave();

	http_enum_opts_init(&h_opts);
	h_opts.progress_update = (void *)&callback_thread_update_progress;
	h_opts.progress_update_data = p_data;
	if (p_data->k_opts->bing_api_key == NULL) {
		gdk_threads_enter();
		gui_popup_error_dialog(p_data->popup_window, "Bing API Key Not Set", "Error: Invalid API Key");
		gui_model_update_marquee((main_gui_data *)p_data, NULL);
		gdk_threads_leave();
	} else {
		http_enum_opts_set_bing_api_key(&h_opts, p_data->k_opts->bing_api_key);
		h_opts.action_status = &p_data->action_status;
		p_data->action_status = KRAKEN_ACTION_RUN;
		response = http_search_engine_bing_ex(p_data->c_host_manager, target_domain, &h_opts);
		gdk_threads_enter();
		if (response < 0) {
			if (response == -3) {
				gui_popup_error_dialog(p_data->popup_window, "Invalid Bing API Key", "Error: Invalid API Key");
			} else {
				GUI_POPUP_ERROR_GENERIC_ERROR(p_data->popup_window);
			}
		}
		gui_model_update_tree_and_marquee((main_gui_data *)p_data, NULL);
		gdk_threads_leave();
	}
	gdk_threads_enter();
	g_signal_handler_disconnect(p_data->popup_window, delete_handler);
	gtk_widget_destroy(p_data->popup_window);
	gdk_threads_leave();
	http_enum_opts_destroy(&h_opts);
	return;
}