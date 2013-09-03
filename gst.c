static void func_tags_handler (const GstTagList * list,  const gchar * tag,  gpointer user_data) {

  int i, num;
  num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i <num; ++i) {
    const GValue *val;
    /* Note: when looking for specific tags, use the g_tag_list_get_xyz() API,
     * we only use the GValue approach here because it is more generic */
    val = gst_tag_list_get_value_index (list, tag, i);
    if (G_VALUE_HOLDS_STRING (val)) {
      g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
    } else if (G_VALUE_HOLDS_UINT (val)) {
      g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
    } else if (G_VALUE_HOLDS_DOUBLE (val)) {
      g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
    } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
      g_print ("\t%20s : %s\n", tag, (g_value_get_boolean (val)) ? "true" : "false");
    } else if (GST_VALUE_HOLDS_BUFFER (val)) {
      GstBuffer *buf = gst_value_get_buffer (val);
    } else if (GST_VALUE_HOLDS_DATE_TIME (val)) {
      GstDateTime *dt = g_value_get_boxed (val);
    } else {
      g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
  }
}
gboolean get_song_position (GstElement *pipeline) {

	GstFormat fmt = GST_FORMAT_TIME;
	char time_string1[50];
	char time_string2[50];

	Guark_data.current_pos = Guark_data.duration = -1;
	gst_element_query_position (Guark_data.pipeline, &fmt, &Guark_data.current_pos);
	gst_element_query_duration (Guark_data.pipeline, &fmt, &Guark_data.duration);

	sprintf(time_string1,"%u:%02u:%02u / ", GST_TIME_ARGS (Guark_data.current_pos));
	sprintf(time_string2,"%u:%02u:%02u", GST_TIME_ARGS (Guark_data.duration));
	if(!strcmp(time_string2,"99:99:99")) strcpy(time_string2,"---");
	strcat(time_string1,time_string2);
	strcpy(Guark_data.timestamp_string,time_string1);
	//g_print("%s",Guark_data.timestamp_string);

	char tooltip_string[200];
	if (Guark_data.state==GUARK_STATE_READY) strcpy(tooltip_string,"STOP. ");
	else if (Guark_data.state==GUARK_STATE_PAUSED) strcpy(tooltip_string,"PAUSE. ");
	else if (Guark_data.state==GUARK_STATE_PLAYING) strcpy(tooltip_string,"PLAY. ");
	strcat(tooltip_string, Guark_data.timestamp_string);
	strcat(tooltip_string, "\n");
	strcat(tooltip_string,Guark_data.playsource);
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

      Playeron_Stop();
			Playeron_Start();
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
    /*
    case GST_MESSAGE_TAG: {
		GstTagList *tags = NULL;
		gst_message_parse_tag (msg, &tags);
		g_print ("Got tags from element: %s\n", GST_OBJECT_NAME (msg->src));
		gst_tag_list_foreach (tags, func_tags_handler, NULL);
		g_print ("\n");
		gst_tag_list_free (tags);
		break;
	}
	*/
    default:
      break;
  }
  return TRUE;
}
static void on_pad_added (GstElement *element, GstPad     *pad, gpointer    data) {
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  sinkpad = gst_element_get_static_pad (decoder, "sink");
  gst_pad_link (pad, sinkpad);
  gst_object_unref (sinkpad);
}
GuarkState Sound_init() {

	if (Guark_data.pipeline) gst_element_set_state (Guark_data.pipeline, GST_STATE_NULL);
	Guark_data.pipeline = gst_pipeline_new ("guark_pipeline");
	bus = gst_pipeline_get_bus (GST_PIPELINE (Guark_data.pipeline));
	watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);
	return GUARK_STATE_READY;
}
GuarkState Sound_Deinit() {

	gst_element_set_state (Guark_data.pipeline, GST_STATE_NULL);
  gst_object_unref (Guark_data.pipeline);
  g_source_remove (watch_id);
	g_main_loop_unref (loop);
	remove("/tmp/guark.status");
	return GUARK_STATE_NULL;
}
GuarkState Sound_Play() {
	// Сначала берем трек из плейлиста, затем устанавливаем его декодеры и запускаем его
	strcpy(Guark_data.playsource,Guark_playlist[Guark_data.playlistpos].track);

	if (Guarkdecoder_set(Guark_data.playsource)==GUARK_STATE_NULL) return GUARK_STATE_NULL; // Устанавливаем декодеры
	Guark_data.state = gst_element_set_state (Guark_data.pipeline, GST_STATE_PLAYING);   // Запускаем файл
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
	g_print ("Now playing: '%s'\n", Guark_data.playsource);
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
