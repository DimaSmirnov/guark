static void print_tags() {

  gst_tag_list_get_string(tags, GST_TAG_ARTIST, &Guark_data.artist);
  gst_tag_list_get_string(tags, GST_TAG_TITLE, &Guark_data.title);
}

gboolean get_song_position (GstElement *pipeline) {

	GstFormat fmt = GST_FORMAT_TIME;
	char time_string1[50];
	char time_string2[50];

	print_tags();

	Guark_data.current_pos = Guark_data.duration = -1;
	gst_element_query_position (Guark_data.pipeline, &fmt, &Guark_data.current_pos);
	gst_element_query_duration (Guark_data.pipeline, &fmt, &Guark_data.duration);

	sprintf(time_string1,"%u:%02u:%02u / ", GST_TIME_ARGS (Guark_data.current_pos));
	sprintf(time_string2,"%u:%02u:%02u", GST_TIME_ARGS (Guark_data.duration));
	if(!strcmp(time_string2,"99:99:99")) strcpy(time_string2,"---");
	strcat(time_string1,time_string2);
	strcpy(Guark_data.timestamp_string,time_string1);

	if (Guark_data.state==GUARK_STATE_READY) strcpy(tooltip_string,"STOP. ");
	else if (Guark_data.state==GUARK_STATE_PAUSED) strcpy(tooltip_string,"PAUSE. ");
	else if (Guark_data.state==GUARK_STATE_PLAYING) strcpy(tooltip_string,"PLAY. ");
	strcat(tooltip_string, Guark_data.timestamp_string);
	strcat(tooltip_string, "\n");
	strcat(tooltip_string,Guark_data.artist);
	strcat(tooltip_string,". ");
	strcat(tooltip_string,Guark_data.title);
	strcat(tooltip_string, " // Guark");
	gtk_status_icon_set_tooltip(tray_icon, tooltip_string); //Update title status

  return TRUE;
}
static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data) {

  GMainLoop *loop = data;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS: {
      g_print ("End-of-stream\n");
      g_main_loop_quit (loop);
      Guark_data.state = GUARK_TRACK_ENDS;

      if(Guark_data.tracktype==GUARK_STREAM) {
				Playeron_Stop();
				Playeron_Start();
			}
			else gtk_menu_item_activate(GTK_MENU_ITEM (menuitem_2)); // Трек закончился, переходим к следующему
      break;
     }
    case GST_MESSAGE_ERROR: {
      gchar *debug = NULL;
      GError *err = NULL;
      gst_message_parse_error (msg, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      if (debug) {
        g_print ("Debug details: %s\n", debug);
        g_free (debug);
      }
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_TAG: {
			GstTagList *newTags = gst_tag_list_new();
			gst_message_parse_tag (msg, &newTags);
			tags=gst_tag_list_merge(tags,newTags,GST_TAG_MERGE_PREPEND);
			gst_tag_list_free (newTags);
			break;
		}
    default: {
			Guark_data.artist="";
			Guark_data.title="";
			break;
		}
  }
  return TRUE;
}

GstElement * Sound_init(char *filename) {

	if (Guark_data.pipeline) gst_element_set_state (Guark_data.pipeline, GST_STATE_NULL);
	if (tags) gst_tag_list_free(tags);
	tags = gst_tag_list_new();
	if (strstr(filename,"http://")) {
		sprintf(temp_string,"playbin2 uri=%s",filename);
		//printf("pipe: %s\n",temp_string);
		Guark_data.pipeline = gst_parse_launch (temp_string, NULL);
	}
	else {
		Guark_data.pipeline  = gst_element_factory_make ("playbin2", "play");
		g_object_set (G_OBJECT (Guark_data.pipeline), "uri", gst_filename_to_uri(filename, &error), NULL);
	}
  bus = gst_pipeline_get_bus (GST_PIPELINE (Guark_data.pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	return Guark_data.pipeline;
}

GuarkState Sound_Deinit() {

	gst_element_set_state (Guark_data.pipeline, GST_STATE_NULL);
	gst_tag_list_free(tags);
  gst_object_unref (Guark_data.pipeline);
	g_main_loop_unref (loop);
	remove("/tmp/guark.status");
	return GUARK_STATE_NULL;
}
GuarkState Sound_Play() {

	strcpy(Guark_data.playsource,Guark_playlist[Guark_data.playlistpos].track); // Get track from playlist
	Guark_data.state = gst_element_set_state (Guark_data.pipeline, GST_STATE_PLAYING);   // Start track
	if (Guark_data.state == GST_STATE_CHANGE_FAILURE) {
		GstMessage *msg;
		g_print ("Failed to start up pipeline!\n");
		msg = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
		if (msg) {
			GError *err = NULL;
			gst_message_parse_error (msg, &err, NULL);
			g_print ("ERROR: %s\n", err->message);
			g_error_free (err);
			gst_message_unref (msg);
		}
    return GUARK_STATE_NULL;
	}
	Guark_data.state = GUARK_STATE_PLAYING;
	//g_print ("Now playing: '%s'\n", Guark_data.playsource);
	return GUARK_STATE_PLAYING;
}
int proc_find(const char* name) {
	DIR* dir;
	struct dirent* ent;
	char buf[512];
	long  pid;
	char state, pname[100] = {0,};
	int cntr=0;
	FILE *fp=NULL;

	if (!(dir = opendir("/proc"))) {
		perror("FATAL: Can't open /proc");
		return -1;
	}
	while((ent = readdir(dir)) != NULL) {
		long lpid = atol(ent->d_name);
		if(lpid < 0) continue;
		snprintf(buf, sizeof(buf), "/proc/%ld/stat", lpid);
		fp = fopen(buf, "r");
		if (fp) {
			if ( (fscanf(fp, "%ld (%[^)]) %c", &pid, pname, &state)) != 3 ){
				printf("FATAL: fail to count program instancies q-ty \n");
				fclose(fp);
				closedir(dir);
				return -1;
			}
			if (!strcmp(pname, name)) cntr++;
		}
	}
	fclose(fp);
	closedir(dir);
return cntr;
}

// vim: noai:ts=2:sw=2
