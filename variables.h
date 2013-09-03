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
	} GuarkDecoderType;

	GuarkState Sound_Play();
	GuarkState Sound_init(int, char**);
	GuarkState Sound_Deinit();
	GuarkState Createmenu();
	GuarkState Playeron_Start();
	GuarkState Playeron_Stop();
	GuarkState Playeron_Changetrack();
	GuarkState Guarkdecoder_set(char*);
	void Guarkplaylist_Show();
	
  GstElement *audio, *filesrc, *decoder, *demuxer, *filter, *sink, *volume;
  GstElement *convert1, *convert2, *resample;
  GMainLoop *loop;
  GstBus *bus;
  guint watch_id;

	GtkWidget *response_widget;
	GtkWidget *menu, *menuitem_1, *menuitem_2, *menuitem_3, *menuitem_4, *menuitem_5, *menuitem_6, *menuitem_7, *menuitem_8;
	GtkWidget *submenu;
	GtkStatusIcon *tray_icon;
	GtkWidget *image;
	GtkWidget *widget;
	GdkPixbuf *buf;

	struct _Guarkplaylist {	//Playlist array
			char track[500];				// Path to track
			char short_track[500];	//Short track name (without / in path)
	} *Guark_playlist;

	typedef struct _Guarkdata {
		GstElement *pipeline;				// guark pipeline
		GstStateChangeReturn state;	// Current state of tracker (GstStateChangeReturn)
		char playsource[255];				// Current song
		int playlistpos;						// Current position in playlist
		int inplaylist;	// Кол-во треков в плейлисте
		int tracktype;							// Type of track (GuarkDecoderType)
		gint64 duration;						// Duration of the song
		gint64 current_pos;					// Current song position
		char timestamp_string[100];
	} Guarkdata;
	Guarkdata Guark_data;



