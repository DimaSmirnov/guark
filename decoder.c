
GuarkState Guarkdecoder_set(char* playsource) { //Устанавливаем декодер

	strcpy(Guark_data.playsource,playsource);

	if (strstr(playsource,"http://")) {
		filesrc  = gst_element_factory_make ("souphttpsrc", "source");
		decoder  = gst_element_factory_make ("mad", "decoder"); // mp3 decoder
		convert1 = gst_element_factory_make ("audioconvert", "converter");
		convert2 = gst_element_factory_make ("audioconvert", "converter2");
		resample = gst_element_factory_make ("audioresample", "resample");
		sink     = gst_element_factory_make ("autoaudiosink", "sink");
		volume = gst_element_factory_make("volume", "volume");
		gst_bin_add_many (GST_BIN (Guark_data.pipeline), filesrc, decoder, convert1, convert2, resample, volume, sink, NULL);
		gst_element_link_many (filesrc, decoder, convert1, convert2, resample, volume, sink, NULL);
		Guark_data.tracktype=GUARK_STREAM;
	}
	else if (strstr(playsource,".mp3")) {
		filesrc  = gst_element_factory_make ("filesrc", "source");
		decoder  = gst_element_factory_make ("mad", "decoder"); // mp3 decoder
		convert1 = gst_element_factory_make ("audioconvert", "converter");
		convert2 = gst_element_factory_make ("audioconvert", "converter2");
		resample = gst_element_factory_make ("audioresample", "resample");
		sink     = gst_element_factory_make ("autoaudiosink", "sink");
		volume = gst_element_factory_make("volume", "volume");
		gst_bin_add_many (GST_BIN (Guark_data.pipeline), filesrc, decoder, convert1, convert2, resample, volume, sink, NULL);
		gst_element_link_many (filesrc, decoder, convert1, convert2, resample, volume, sink, NULL);
		Guark_data.tracktype=GUARK_MP3;
	}
	else if (strstr(playsource,".ogg")) {
		filesrc  = gst_element_factory_make ("filesrc", "file-source");
		demuxer  = gst_element_factory_make ("oggdemux",      "ogg-demuxer");
		decoder  = gst_element_factory_make ("vorbisdec",     "vorbis-decoder");
		convert1 = gst_element_factory_make ("audioconvert",  "converter");
		volume	 = gst_element_factory_make ("volume", "volume");
		sink     = gst_element_factory_make ("autoaudiosink", "audio-output");
		gst_bin_add_many (GST_BIN (Guark_data.pipeline), filesrc, demuxer, decoder, convert1, volume, sink, NULL);
		gst_element_link (filesrc, demuxer);
		gst_element_link_many (decoder, convert1, volume, sink, NULL);
		g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), decoder);
		Guark_data.tracktype=GUARK_OGG;
	}
	else return GUARK_STATE_NULL;

g_object_set (G_OBJECT (filesrc), "location", playsource, NULL);
return GUARK_STATE_READY;
}

