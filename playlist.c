
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

	gtk_widget_set_sensitive (menuitem_3,TRUE);
	menuitem_8 = gtk_image_menu_item_new_with_label(trackpath); // Show track from playlist
	buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL);
	image = gtk_image_new_from_pixbuf(buf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_8), image);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem_8);
	g_signal_connect(menuitem_8, "activate",(GCallback) Playeron_Trackselect, widget);

	gtk_widget_show_all(submenu);

	return Guark_data.inplaylist;
}

void Guarkplaylist_Init() {

	Guark_data.inplaylist=0; //Кол-во треков в плейлисте
}
