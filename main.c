#include <string.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gdk/gdk.h>


#include "variables.h"
#include "gst.c"
#include "gtk.c"


int main (int argc, char *argv[]) {

	Guark_data.state = Sound_init(argc, &argv[0]);
	Guark_data.pipeline = gst_pipeline_new ("guark_pipeline");
	bus = gst_pipeline_get_bus (GST_PIPELINE (Guark_data.pipeline));
	watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);
	if (argv[1]) {
		if (strstr(argv[1],"http://")) { filesrc  = gst_element_factory_make ("souphttpsrc", "source"); Guark_data.tracktype=GUARK_STREAM; }
		else { filesrc  = gst_element_factory_make ("filesrc", "my_filesource"); Guark_data.tracktype=GUARK_MP3;}
		strcpy(Guark_data.playsource,argv[1]);
	}
	else {
		filesrc  = gst_element_factory_make ("souphttpsrc", "source");
		strcpy(Guark_data.playsource,"http://uk2.mrgigabit.com:8000");
		Guark_data.tracktype=GUARK_STREAM;
	}
	Guark_data.state = Sound_Play();
	g_print ("Now playing: %s\n", Guark_data.playsource);

	gtk_init(&argc, &argv);
	tray_icon = CreateTrayIcon();
	GuarkState ret = Createmenu();

	gtk_main();

  gst_element_set_state (Guark_data.pipeline, GST_STATE_NULL);
  gst_object_unref (Guark_data.pipeline);
  g_source_remove (watch_id);
	Guark_data.state = Sound_Deinit();
  return 0;
}
