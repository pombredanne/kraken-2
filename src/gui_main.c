// gui_main.c
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

#include "kraken.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <arpa/inet.h>

#include "gui_menu_functions.h"
#include "gui_model.h"
#include "gui_main.h"
#include "gui_popups.h"
#include "plugins.h"
#include "host_manager.h"
#include "whois_lookup.h"

#define GUI_MAIN_MAX_CMD_SZ 511

void callback_main_command_reset_color(GtkWidget *widget, main_gui_data *m_data) {
	gtk_widget_modify_base(widget, GTK_STATE_NORMAL, NULL);
	return;
}

void callback_thread_main_command_submit(main_gui_data *m_data) {
	const gchar *text;
	GdkColor color;
	plugin_object *plugin;
	char buffer[GUI_MAIN_MAX_CMD_SZ + 1];
	char error_msg[64];
	char *args = NULL;
	char *command = buffer;
	char *p = buffer;
	kstatus_plugin ret_val;

	gdk_threads_enter();
	text = gtk_entry_get_text(GTK_ENTRY(m_data->plugin_entry));
	if (text == NULL) {
		gdk_threads_leave();
		kraken_thread_mutex_unlock(&m_data->plugin_mutex);
		return;
	}
	memset(buffer, '\0', sizeof(buffer));
	strncpy(buffer, text, GUI_MAIN_MAX_CMD_SZ);

	while ((*p != ' ') && (*p != '\0') && ((command - p - 1) < GUI_MAIN_MAX_CMD_SZ)) {
		p++;
	}
	*p = '\0';
	args = p + 1;
	if (strlen(command) == 0) {
		gdk_threads_leave();
		kraken_thread_mutex_unlock(&m_data->plugin_mutex);
		return;
	}

	if ((strlen(command) > PLUGIN_SZ_NAME) || (!plugins_get_plugin_by_name(command, &plugin))) {
		gdk_color_parse("#FF0000", &color);
		gtk_widget_modify_base(m_data->plugin_entry, GTK_STATE_NORMAL, &color);
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
		gdk_threads_leave();
		kraken_thread_mutex_unlock(&m_data->plugin_mutex);
		return;
	}

	gui_model_update_marquee(m_data, "Running Plugin");
	gtk_entry_set_editable(GTK_ENTRY(m_data->plugin_entry), FALSE);
	gdk_color_parse("#00FF00", &color);
	gtk_widget_modify_base(m_data->plugin_entry, GTK_STATE_NORMAL, &color);
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
	gdk_threads_leave();

	ret_val = plugins_run_plugin_method_arg_str(plugin, PLUGIN_METHOD_MAIN, args, error_msg, sizeof(error_msg));
	whois_fill_host_manager(m_data->c_host_manager);

	if (m_data->gui_is_active) {
		gdk_threads_enter();
		if (KSTATUS_PLUGIN_IS_ERROR(ret_val)) {
			gdk_color_parse("#FF6600", &color);
			gtk_widget_modify_base(m_data->plugin_entry, GTK_STATE_NORMAL, &color);
			gui_popup_error_dialog_plugin(m_data->main_window, ret_val, error_msg);
		} else {
			gtk_entry_set_text(GTK_ENTRY(m_data->plugin_entry), "");
			gtk_widget_modify_base(m_data->plugin_entry, GTK_STATE_NORMAL, NULL);
		}
		gtk_entry_set_editable(GTK_ENTRY(m_data->plugin_entry), TRUE);
		gui_model_update_tree_and_marquee(m_data, NULL);
		gdk_threads_leave();
	}

	kraken_thread_mutex_unlock(&m_data->plugin_mutex);
	return;
}

void callback_main_command_submit(GtkWidget *widget, main_gui_data *m_data) {
	if (kraken_thread_mutex_trylock(&m_data->plugin_mutex) == 0) {
		kraken_thread_create(&m_data->plugin_thread, callback_thread_main_command_submit, m_data);
	} else {
		GUI_POPUP_ERROR_PLUGIN_RUNNING(m_data->main_window);
	}
	return;
}

void gui_main_data_init(main_gui_data *m_data, kraken_opts *k_opts, host_manager *c_host_manager) {
	memset(m_data, '\0', sizeof(main_gui_data));
	m_data->main_window = NULL;
	m_data->k_opts = k_opts;
	m_data->c_host_manager = c_host_manager;
	kraken_thread_mutex_init(&m_data->plugin_mutex);
	m_data->gui_is_active = 0;
	return;
}

void gui_main_data_destroy(main_gui_data *m_data) {
	m_data->k_opts = NULL;
	m_data->c_host_manager = NULL;
	m_data->gui_is_active = 0;
	kraken_thread_mutex_destroy(&m_data->plugin_mutex);
	return;
}

int gui_show_main_window(main_gui_data *m_data) {
	GtkWidget *window;
	GtkWidget *scroll_window;
	GtkWidget *main_vbox, *hbox;
	GtkWidget *main_menu_bar;
	GtkWidget *view;
	GtkWidget *txt_entry;

#ifdef HAVE_GTK_G_THREAD_INIT
	g_thread_init(NULL);
#endif
	gdk_threads_init();
	gdk_threads_enter();
	gtk_init(NULL, NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	m_data->main_window = window;
	g_signal_connect(window, "delete-event", gtk_main_quit, NULL); /* dirty */
	gtk_window_set_title(GTK_WINDOW(window), "Kraken");
	gtk_container_set_border_width(GTK_CONTAINER(window), 0);
	gtk_widget_set_size_request(window, 550, 600);

	main_vbox = gtk_vbox_new(FALSE, 1);
	gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 1);
	gtk_container_add(GTK_CONTAINER(window), main_vbox);

	scroll_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(scroll_window), 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	view = gui_model_create_view_and_model(m_data->c_host_manager, m_data);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_window), view);

	m_data->tree_view = view;
	m_data->plugin_box = gtk_hbox_new(FALSE, 0);
	main_menu_bar = gui_menu_get_main_menubar(window, m_data);

	hbox = gtk_hbox_new(FALSE, 0);
	m_data->main_marquee = hbox;
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);

	txt_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(txt_entry), GUI_MAIN_MAX_CMD_SZ);
	g_signal_connect(txt_entry, "changed", G_CALLBACK(callback_main_command_reset_color), m_data);
	g_signal_connect(txt_entry, "activate", G_CALLBACK(callback_main_command_submit), m_data);

	gtk_box_pack_start(GTK_BOX(main_vbox), main_menu_bar, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), scroll_window, TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);

	gtk_container_set_border_width(GTK_CONTAINER(m_data->plugin_box), 2);
	gtk_box_pack_start(GTK_BOX(m_data->plugin_box), txt_entry, TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(main_vbox), m_data->plugin_box, FALSE, FALSE, 0);

	m_data->plugin_entry = txt_entry;

	gtk_widget_show_all(window);
	gui_model_update_tree_and_marquee(m_data, NULL);

	m_data->gui_is_active = 1;
	gtk_main();
	gdk_threads_leave();
	return 0;
}
