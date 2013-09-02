void ActionTrayIcon_OnClick(GtkStatusIcon *status_icon, gpointer user_data) { // Click on tray icon

	if (Guark_data.state==3) { // Stay on pause
		Playeron_Start();
	}
	else if (Guark_data.state==4) { // Stay on play
		gst_element_set_state (Guark_data.pipeline, GST_STATE_PAUSED);
		Guark_data.state=3;
		printf("Paused\n");
		}
}

gboolean ActionTrayIcon_OnScroll (GtkWidget *widget, GdkEventScroll *event, gpointer user_data) { // mouse scroll on tray icon
	double vol;
	GstElement *volume;

	volume = gst_bin_get_by_name(GST_BIN(Guark_data.pipeline), "volume");
	g_object_get (G_OBJECT(volume), "volume", &vol, NULL);

	if (event->direction == GDK_SCROLL_UP) { // Volume UP
		if (vol<1.4) {
			vol = vol+0.1;
			g_object_set (G_OBJECT(volume), "volume", vol, NULL);
		}
		printf("Volume up to: %f\n", vol);
	}
	else if (event->direction == GDK_SCROLL_DOWN) { // Volume down
			vol = vol-0.1;
			if (vol<=0) g_object_set (G_OBJECT(volume), "volume", 0);
			else g_object_set (G_OBJECT(volume), "volume", vol, NULL);
			printf("Volume down to: %f\n", vol);
	}
	else if (event->direction == GDK_SCROLL_RIGHT) { // Next track in playlist
		printf("Next track in playlist\n");
	}
	else if (event->direction == GDK_SCROLL_LEFT) { // Prev. track in playlist
		printf("Previously track in playlist\n");
	}

	gst_object_unref(volume);
}

void Playeron_Quit() {
	printf("Bye-bye\n");
	gtk_main_quit ();
}
void Playeron_Prevsong() {

	printf("Prev song\n");
}
void Playeron_Nextsong() {

	printf("Next song\n");
}
GuarkState Playeron_Start() {
	gst_element_set_state (Guark_data.pipeline, GST_STATE_PLAYING);
	Guark_data.state=GUARK_STATE_PLAYING;
	printf("Start playing\n");
	return GUARK_STATE_PLAYING;
}
GuarkState Playeron_Stop() {
	gst_element_set_state (Guark_data.pipeline, GST_STATE_READY);
	Guark_data.state=2;
	printf("Stop playing\n");
}
GuarkState Playeron_Changetrack() {
	printf("Change track or stream off\n");
	if (Guark_data.tracktype == GUARK_STREAM) { //if stream fail - restart
		Playeron_Stop();
		Playeron_Start();
	}
}
GuarkState Createmenu() {

	GtkWidget *submenu, *menuitem_subitem;

	menu = gtk_menu_new();
	menuitem_1 = gtk_image_menu_item_new_with_label("Previously");
	menuitem_2 = gtk_image_menu_item_new_with_label("Next");
	menuitem_3 = gtk_image_menu_item_new_with_label("Playlist");
	menuitem_4 = gtk_image_menu_item_new_with_label("Play");
	menuitem_5 = gtk_image_menu_item_new_with_label("Stop");
	menuitem_6 = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL); // GTK_STOCK_MEDIA_PAUSE, GTK_STOCK_MEDIA_STOP

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL); // Submenu is here!
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_3), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_3);

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/rewind.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_1), image);
	g_signal_connect(menuitem_1, "activate",(GCallback) Playeron_Prevsong, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_1);
	gtk_widget_set_sensitive (menuitem_1,FALSE); // gray-out menu item

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/forward.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_2), image);
	g_signal_connect(menuitem_2, "activate",(GCallback) Playeron_Nextsong, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_2);
	gtk_widget_set_sensitive (menuitem_2,FALSE); // gray-out menu item

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/play.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_4), image);
	g_signal_connect(menuitem_4, "activate",(GCallback) Playeron_Start, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_4);

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/stop.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_5), image);
	g_signal_connect(menuitem_5, "activate",(GCallback) Playeron_Stop, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_5);

	g_signal_connect(menuitem_6, "activate",(GCallback) Playeron_Quit, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_6);

	gtk_widget_show_all(menu);


	submenu = gtk_menu_new(); // Submenu init
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(menuitem_3), submenu);
	gtk_widget_set_sensitive (menuitem_3,FALSE); // gray-out submenu. it is TEST option temporary

	menuitem_7 = gtk_image_menu_item_new_with_label("Clear playlist"); // Submenu starts here
	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_7), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem_7);


	gtk_widget_show_all(submenu);
	return 0;
}
void ActionTrayIcon_OnMenu(GtkWidget *widget, GdkEvent *event) { // Show main menu
	gtk_menu_popup (GTK_MENU(menu), 0, NULL, NULL, NULL, 0, gtk_get_current_event_time ());
}
static GtkStatusIcon *Guark_Init() {

				Guark_playlist = calloc(1, sizeof(struct _Guarkplaylist));

        tray_icon = gtk_status_icon_new();
        g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(ActionTrayIcon_OnMenu), NULL);
        g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(ActionTrayIcon_OnClick), NULL);
        g_signal_connect(G_OBJECT(tray_icon), "scroll-event",G_CALLBACK(ActionTrayIcon_OnScroll), NULL);
        gtk_status_icon_set_from_file (tray_icon, "/usr/share/pixmaps/guark/guark.png");
        gtk_status_icon_set_tooltip(tray_icon, "Guark. Simple audio player");
        gtk_status_icon_set_visible(tray_icon, TRUE);
        return tray_icon;
}

