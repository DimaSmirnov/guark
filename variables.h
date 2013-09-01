	typedef enum {
		GUARK_STATE_NULL    = 1,
		GUARK_STATE_READY   = 2,
		GUARK_STATE_PAUSED  = 3,
		GUARK_STATE_PLAYING = 4,
		GUARK_TRACK_ENDS 		= 5
	} GuarkState;

	typedef enum {
		GUARK_MP3    = 1,
		GUARK_STREAM   = 2,
		GUARK_OGG  = 3
	} GuarkTrackType;

	GuarkState Sound_Play();
	GuarkState Sound_init(int, char**);
	GuarkState Sound_Deinit();
	GuarkState Createmenu();
	GuarkState Playeron_Start();
	GuarkState Playeron_Stop();
	GuarkState Playeron_Changetrack();

  GstElement *filesrc, *decoder, *filter, *sink;
  GstElement *convert1, *convert2, *resample;
  GMainLoop *loop;
  GstBus *bus;
  guint watch_id;

	GtkWidget *response_widget;
	GtkWidget *menu, *menuitem_1, *menuitem_2, *menuitem_3, *menuitem_4, *menuitem_5, *menuitem_6;
	GtkStatusIcon *tray_icon;
	GtkWidget *image;
	GtkWidget *widget;
	GdkPixbuf *buf;

	typedef struct _Guarkdata {
		GstElement *pipeline;				// guark pipeline
		GstStateChangeReturn state;	// Current state of the pipeline
		char playsource[255];			// Current song;
		int tracktype;							// Type of track (GuarkTrackType)
		int duration;								// Duration of the song, in seconds
		int current_pos;						// Current song position
	} Guarkdata;
	Guarkdata Guark_data;



