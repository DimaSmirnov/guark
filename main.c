#include <string.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gdk/gdk.h>


#include "variables.h"
#include "gst.c"
#include "gtk.c"


int main (int argc, char *argv[]) {

	int ret = Sound_init(argc, &argv[0]);
	pipeline = gst_pipeline_new ("guark_pipeline");
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	//if (strstr(argv[1],"http://"))
	filesrc  = gst_element_factory_make ("souphttpsrc", "source");
	//else filesrc  = gst_element_factory_make ("filesrc", "my_filesource");
	//strcpy(play_source,argv[1]);
	strcpy(play_source,"http://uk2.mrgigabit.com:8000");
	ret = Sound_Play();
	g_print ("Now playing: %s\n", play_source);

	gtk_init(&argc, &argv);
	tray_icon = CreateTrayIcon();
	gtk_main();

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_source_remove (watch_id);

	ret = Sound_Deinit();
  return 0;
}
