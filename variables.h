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
	GstElement * Sound_init(char*);
	GuarkState Sound_Deinit();
	GuarkState Createmenu();
	GuarkState Playeron_Start();
	GuarkState Playeron_Stop();
	GuarkState Playeron_Changetrack();
	GuarkState Guarkdecoder_set(char*);
	void Guarkplaylist_Show();
	int Guarkplaylist_Read();
	int Guarkplaylist_CheckUpdateStatus();
	int Guarkplaylist_PlayPrev();
	int Guarkplaylist_PlayNext();

  GstElement *sink, *volume;
  GstBus *bus;
  GstTagList *tags;
	GError *error = NULL;
  guint watch_id;
	GstElement *playbin;
	GMainLoop *loop;

	GtkWidget *response_widget;
	GtkWidget *menu, *menuitem_1, *menuitem_2, *menuitem_3, *menuitem_4, *menuitem_5, *menuitem_6, *menuitem_7, *menuitem_8;
	GtkWidget *submenu;
	GtkStatusIcon *tray_icon;
	GtkWidget *image;
	GtkWidget *widget;
	GdkPixbuf *buf;


	FILE *pFile,*pFile1;
	char temp_string[500];
	char tooltip_string[200];

	struct _Guarkplaylist {	//Playlist array
			char track[500];				// Path to track
			//gchar *artist;
			//gchar *title;
			//char short_track[500];	//Short track name (without / in path)
	} *Guark_playlist;

	typedef struct _Guarkdata {
		GstElement *pipeline;				// guark pipeline
		GstStateChangeReturn state;	// Current state of tracker (GstStateChangeReturn)
		char playsource[500];				// Current song
		int playlistpos;						// Current position in playlist
		gchar *artist;
		gchar *title;
		int inplaylist;							// Кол-во треков в плейлисте
		int tracktype;							// Type of track (GuarkDecoderType)
		gint64 duration;						// Duration of the song
		gint64 current_pos;					// Current song position
		char timestamp_string[100];
	} Guarkdata;
	Guarkdata Guark_data;



