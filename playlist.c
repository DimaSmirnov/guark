
int Guarkplaylist_PlayNext() {

	if (Guark_data.playlistpos < Guark_data.inplaylist-1) Guark_data.playlistpos++;
	else Guark_data.playlistpos=0;
	Sound_init(Guark_playlist[Guark_data.playlistpos].track);

	Guark_data.state = Sound_Play();
	Guarkplaylist_Show();
	return Guark_data.playlistpos;
}

int Guarkplaylist_PlayPrev() {
	if (Guark_data.playlistpos > 0)Guark_data.playlistpos--;
	else Guark_data.playlistpos=Guark_data.inplaylist-1;
	Sound_init(Guark_playlist[Guark_data.playlistpos].track);
	Guark_data.state = Sound_Play();
	Guarkplaylist_Show();
	return Guark_data.playlistpos;
}

int Guarkplaylist_addInto(char* trackpath) {
	if (!strstr(trackpath,".mp3") &&
	!strstr(trackpath,".ogg") &&
	!strstr(trackpath,"http://")) return 1; // Если подсунули неверный файл, то выходим с единицей

	pFile1=fopen("/tmp/guark.playlist","a");
	if (pFile1) {
		sprintf(temp_string,"%s\n",trackpath);
		fputs (temp_string,pFile1);
		fclose(pFile1);
		pFile=fopen("/tmp/guark.status","a");
		fclose(pFile);
	}
	return 0;
}
void Guarkplaylist_Clear() {

	printf("Clear playlist\n");
	remove("/tmp/guark.playlist");
	Guarkplaylist_Read();
	Createmenu();
	Guarkplaylist_Show();
}
int Guarkplaylist_CheckUpdateStatus() {

	pFile=fopen("/tmp/guark.status","r");
	if (pFile != NULL) { // Need to update playlist
		fclose(pFile);
		remove("/tmp/guark.status");
		Guark_data.inplaylist = Guarkplaylist_Read();
		Guarkplaylist_Show();
		Guark_data.playlistpos = Guark_data.inplaylist-1; // Start added track
		Sound_init(Guark_playlist[Guark_data.playlistpos].track);
		GuarkState ret = Createmenu();
		Guark_data.state = Sound_Play();
	}
	return TRUE;
}
int Guarkplaylist_Read() {

	Guark_data.inplaylist=0;
	pFile1=fopen("/tmp/guark.playlist","r");
	if (!pFile1) return 0;
	while (!feof(pFile1)) {  // Count lines q-ty in playlist file
		fgets (temp_string , 500 , pFile1);
		Guark_data.inplaylist++;
	}
	rewind(pFile1);
	Guark_data.inplaylist--;
	Guark_playlist = realloc(Guark_playlist, (Guark_data.inplaylist+1) * sizeof(struct _Guarkplaylist));
	for (int i=0;i<Guark_data.inplaylist;i++) {
		fgets (temp_string , 500 , pFile1);
		temp_string[strlen(temp_string)-1] = '\0';
		strcpy(Guark_playlist[i].track,temp_string);
	}
	fclose(pFile1);

	return Guark_data.inplaylist;
}

int Guarkplaylist_Trackselect(GtkMenuItem *widget, gpointer user_data) {

	strcpy(Guark_data.playsource,gtk_menu_item_get_label(widget));
	strcpy(temp_string,strtok(Guark_data.playsource,": "));
	Guark_data.playlistpos = atoi(temp_string)-1;
	Sound_init(Guark_playlist[Guark_data.playlistpos].track);
	Guark_data.state = Sound_Play();
	Guarkplaylist_Show();
	return 0;
}
void Guarkplaylist_Show() {

	if (submenu) gtk_menu_detach((GtkMenu*)submenu);
	submenu = gtk_menu_new(); // Submenu init
	gtk_menu_item_set_submenu (GTK_MENU_ITEM(menuitem_3), submenu);

  menuitem_7 = gtk_image_menu_item_new_with_label("Clear playlist"); // Submenu starts here
  buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/guark.png", 16,16, NULL);
  image = gtk_image_new_from_pixbuf(buf);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_7), image);
  gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem_7);
  g_signal_connect(menuitem_7, "activate",(GCallback)Guarkplaylist_Clear, widget);
  if (Guark_data.inplaylist>0) gtk_widget_set_sensitive (menuitem_3,TRUE); // enable playlist
	else gtk_widget_set_sensitive (menuitem_3,FALSE); // disable playlist

	for (int i=0; i<Guark_data.inplaylist; i++) {
		sprintf(temp_string,"%d: %s",i+1,Guark_playlist[i].track);
		if (Guark_data.playlistpos==i) {
  		menuitem_8 = gtk_image_menu_item_new_with_label(temp_string); // Show tracks from playlist
  		buf = gdk_pixbuf_new_from_file_at_size("/usr/share/pixmaps/guark/play.png", 16,16, NULL);
  		image = gtk_image_new_from_pixbuf(buf);
  		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menuitem_8), image);
		}
		else {
			menuitem_8 = gtk_menu_item_new_with_label(temp_string);
		}
  	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem_8);
  	g_signal_connect(menuitem_8, "activate",(GCallback) Guarkplaylist_Trackselect, widget);
	}
	gtk_widget_show_all(submenu);
}

// vim: noai:ts=2:sw=2:syntax=c
