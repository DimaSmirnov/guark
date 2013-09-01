void ActionTrayIcon_OnClick(GtkStatusIcon *status_icon, gpointer user_data) // Click on tray icon
{
	if (gst_ret==3) {
		gst_element_set_state (pipeline, GST_STATE_PLAYING);
		gst_ret=4;
		gtk_status_icon_set_tooltip(tray_icon, "Play.  Guark-simple audio player");
		//printf("Playing\n");
	}
	else if (gst_ret==4) {
		gst_element_set_state (pipeline, GST_STATE_PAUSED);
		gst_ret=3;
		gtk_status_icon_set_tooltip(tray_icon, "Pause.  Guark-simple audio player");
		//printf("Paused\n");
		}
}
void ActionOnQuit()
{
	printf("Quit !!!\n");
	gtk_main_quit ();
}
void ActionTrayIcon_OnMenu(GtkWidget *widget, GdkEvent *event) // Show main menu
{
	GtkWidget *menuitem, *image;
	GdkPixbuf *buf;

	menu = gtk_menu_new();


	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/rewind.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	menuitem = gtk_image_menu_item_new_with_label("Previously");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/forward.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	menuitem = gtk_image_menu_item_new_with_label("Next");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/play.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	menuitem = gtk_image_menu_item_new_with_label("Play");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/stop.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	menuitem = gtk_image_menu_item_new_with_label("Stop");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL); // GTK_STOCK_MEDIA_PAUSE, GTK_STOCK_MEDIA_STOP
	g_signal_connect(menuitem, "activate",(GCallback) ActionOnQuit, widget);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	gtk_widget_show_all(menu);

	gtk_menu_popup (GTK_MENU(menu), 0, NULL, NULL, NULL, 0, gtk_get_current_event_time ());
}
static GtkStatusIcon *CreateTrayIcon()
{
        tray_icon = gtk_status_icon_new();
        g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(ActionTrayIcon_OnMenu), NULL);
        g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(ActionTrayIcon_OnClick), NULL);
        gtk_status_icon_set_from_file (tray_icon, "/usr/share/pixmaps/guark/guark.png");
        gtk_status_icon_set_tooltip(tray_icon, "Guark. Simple audio player");
        gtk_status_icon_set_visible(tray_icon, TRUE);
        return tray_icon;
}

