
	int Sound_Play();
	int Sound_init(int, char**);
	int Sound_Deinit();

	GstStateChangeReturn gst_ret;
  GstElement *pipeline, *filesrc, *decoder, *filter, *sink;
  GstElement *convert1, *convert2, *resample;
  GMainLoop *loop;
  GstBus *bus;
  guint watch_id;

	GtkWidget *response_widget;
	GtkWidget *menu;
	GtkStatusIcon *tray_icon;


	char play_source[200];

	typedef struct _GuarkData {
		GstElement *playbin2;           /* Our one and only pipeline */

		GtkWidget *slider;              /* Slider widget to keep track of current position */
		GtkWidget *streams_list;        /* Text widget to display info about the streams */
		gulong slider_update_signal_id; /* Signal ID for the slider update signal */

		GstState state;                 /* Current state of the pipeline */
		gint64 duration;                /* Duration of the clip, in nanoseconds */
	} GuarkData;
	GuarkData data;
