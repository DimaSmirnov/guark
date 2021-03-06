void Playeron_Pause() { // Pause button event

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

	if (event->direction == GDK_SCROLL_UP) { // scroll UP
		gst_element_seek_simple(Guark_data.pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, Guark_data.current_pos + 1e10);
	}
	else if (event->direction == GDK_SCROLL_DOWN) { // scroll down
		gst_element_seek_simple(Guark_data.pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, Guark_data.current_pos - 1e10);
	}
	else if (event->direction == GDK_SCROLL_RIGHT) { // Next track in playlist
		printf("Next track in playlist\n");
	}
	else if (event->direction == GDK_SCROLL_LEFT) { // Prev. track in playlist
		printf("Previously track in playlist\n");
	}
}
void Playeron_Quit() {
	printf("Bye-bye\n");
	gtk_main_quit ();
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
GuarkState Createmenu() {

	menu = gtk_menu_new();
	menuitem_1 = gtk_image_menu_item_new_with_label("Previously");
	menuitem_2 = gtk_image_menu_item_new_with_label("Next");
	menuitem_3 = gtk_image_menu_item_new_with_label("Playlist");
	menuitem_4 = gtk_image_menu_item_new_with_label("Play");
	menuitem_5 = gtk_image_menu_item_new_with_label("Stop");
	menuitem_7 = gtk_image_menu_item_new_with_label("Pause");
	menuitem_6 = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL); // GTK_STOCK_MEDIA_PAUSE, GTK_STOCK_MEDIA_STOP

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL); // Submenu is here!
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_3), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_3);
	gtk_widget_set_sensitive (menuitem_3,FALSE); // diable playlist

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/rewind.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_1), image);
	g_signal_connect(menuitem_1, "activate",(GCallback) Guarkplaylist_PlayPrev, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_1);
  if (Guark_data.inplaylist>1) gtk_widget_set_sensitive (menuitem_1,TRUE); // enable playlist
	else gtk_widget_set_sensitive (menuitem_1,FALSE); // disable playlist

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/forward.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_2), image);
	g_signal_connect(menuitem_2, "activate",(GCallback) Guarkplaylist_PlayNext, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_2);
  if (Guark_data.inplaylist>1) gtk_widget_set_sensitive (menuitem_2,TRUE); // enable playlist
	else gtk_widget_set_sensitive (menuitem_2,FALSE); // disable playlist

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

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/pause.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_7), image);
	g_signal_connect(menuitem_7, "activate",(GCallback) Playeron_Pause, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_7);

	g_signal_connect(menuitem_6, "activate",(GCallback) Playeron_Quit, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_6);
	gtk_widget_show_all(menu);

	Guarkplaylist_Show();

	g_timeout_add (1000, (GSourceFunc) get_song_position, Guark_data.pipeline);
	g_timeout_add (600, (GSourceFunc) Guarkplaylist_CheckUpdateStatus, Guark_data.pipeline);

	return 0;
}
void ActionTrayIcon_OnMenu(GtkWidget *widget, GdkEvent *event) { // Show main menu
	gtk_menu_popup (GTK_MENU(menu), 0, NULL, NULL, NULL, 0, gtk_get_current_event_time ());
}
static GtkStatusIcon *Guark_Init(int argc, char *argv[]) {

	Guark_playlist = calloc(1, sizeof(struct _Guarkplaylist));
	Guark_data.inplaylist = Guarkplaylist_Read();
	if (!argv[1] && !Guark_data.inplaylist) {
		strcpy(guark_notify,"Playlist is empty");
		strcpy(guark_notify_header,"Guark notification");
		printf("%s\n",guark_notify);
		Create_notify(guark_notify, guark_notify_header,0);
		return 0;
	}
	Guark_data.artist = "Artist";
	Guark_data.title = "Music";

	gtk_init(&argc, &argv);

	tray_icon = gtk_status_icon_new();
	g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(ActionTrayIcon_OnMenu), NULL);
	g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(ActionTrayIcon_OnMenu), NULL);
	g_signal_connect(G_OBJECT(tray_icon), "scroll-event",G_CALLBACK(ActionTrayIcon_OnScroll), NULL);
	gtk_status_icon_set_from_file (tray_icon, "/usr/share/pixmaps/guark/guark.png");
	gtk_status_icon_set_tooltip(tray_icon, "Guark. Simple audio player");
	gtk_status_icon_set_visible(tray_icon, TRUE);
	return tray_icon;
}

// vim: noai:ts=2:sw=2
