
int Guarkplaylist_getNext() {
	int i = Guark_data.playlistpos;
	i++;
	return i;
}

int Guarkplaylist_getPrev() {
	int i = Guark_data.playlistpos;
	i--;
	return i;
}

int Guarkplaylist_addInto(char* trackpath) {

	Guark_playlist = realloc(Guark_playlist, (Guark_data.inplaylist+1) * sizeof(struct _Guarkplaylist));
	strcpy(Guark_data.playsource,trackpath);
	strcpy(Guark_playlist[Guark_data.inplaylist].track,trackpath);
	Guark_data.inplaylist++;
	Guarkplaylist_Show();

	return Guark_data.inplaylist;
}

void Guarkplaylist_Init() {

	Guark_data.inplaylist=0; //Кол-во треков в плейлисте
}
void Guarkplaylist_Show() {
	
	gtk_menu_detach((GtkMenu*)submenu);
	submenu = gtk_menu_new(); // Submenu init
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(menuitem_3), submenu);

  menuitem_7 = gtk_image_menu_item_new_with_label("Clear playlist"); // Submenu starts here
  buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL);
  image = gtk_image_new_from_pixbuf(buf);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_7), image);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem_7);
  g_signal_connect(menuitem_7, "activate",(GCallback) Playeron_ClearPlaylist, widget);
  gtk_widget_set_sensitive (menuitem_3,TRUE); // enable playlist

	for (int i=0; i<Guark_data.inplaylist; i++) {
  	menuitem_8 = gtk_image_menu_item_new_with_label(Guark_playlist[i].track); // Show tracks from playlist
  	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL);
  	image = gtk_image_new_from_pixbuf(buf);
  	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_8), image);
  	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem_8);
  	g_signal_connect(menuitem_8, "activate",(GCallback) Playeron_Trackselect, widget);
	}
	gtk_widget_show_all(submenu);
}

// vim: noai:ts=2:sw=2:syntax=c
