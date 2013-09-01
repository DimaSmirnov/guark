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
	strcat(tooltip_string, "\nGuark");
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
      Playeron_Changetrack();
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

GuarkState Sound_init(int argc, char *argv[])
{
	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);

	return GUARK_STATE_READY;
}
GuarkState Sound_Deinit()
{
	g_main_loop_unref (loop);
	return GUARK_STATE_NULL;
}
GuarkState Sound_Play() {

	//audio = gst_bin_new ("audiobin");

	decoder  = gst_element_factory_make ("mad", "decoder"); // mp3 decoder
	convert1 = gst_element_factory_make ("audioconvert", "converter_1");
	convert2 = gst_element_factory_make ("audioconvert", "converter_2");
	resample = gst_element_factory_make ("audioresample", "resample");
	sink     = gst_element_factory_make ("autoaudiosink", "sink");

	volume = gst_element_factory_make("volume", "volume");

	if (!sink || !decoder) {
		g_print ("FATAL: Decoder or output could not be found. Gstreamer error\n");
		return GUARK_STATE_NULL;
	} else if (!convert1 || !convert2 || !resample) {
		g_print ("FATAL: Could not create audioconvert or audioresample element. Gstreamer error\n");
		return GUARK_STATE_NULL;
	}

	g_object_set (G_OBJECT (filesrc), "location", Guark_data.playsource, NULL);
	gst_bin_add_many (GST_BIN (Guark_data.pipeline), filesrc, decoder, convert1, convert2, resample, volume, sink, NULL);

	if (!gst_element_link_many (filesrc, decoder, convert1, convert2, resample, volume, sink, NULL)) {
		g_print ("FATAL: Failed to link one or more elements! Audio settings error\n");
		return GUARK_STATE_NULL;
	}

	Guark_data.state = gst_element_set_state (Guark_data.pipeline, GST_STATE_PLAYING);   // Play!
	if (Guark_data.state == GST_STATE_CHANGE_FAILURE) { // Failed to start play
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

//

	Guark_data.state = GUARK_STATE_PLAYING;
	return GUARK_STATE_PLAYING;
}

