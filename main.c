#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gdk/gdk.h>

#include "variables.h"
#include "gst.c"
#include "decoder.c"
#include "gtk.c"
#include "playlist.c"


int main (int argc, char *argv[]) {

	remove("/tmp/guark.status");
	if (proc_find("guark")==2) { // Если пытаемся запустить еще 1 гварк
		if (argv[1]) {
			Guarkplaylist_addInto(argv[1]); // Добавляем файл argv[1] в плейлист
		}
		return 0;
	}
	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);
	Guark_data.state = Sound_init();
	int i = Guarkplaylist_Read();
	tray_icon = Guark_Init(argc, &argv[0]);
	if (!tray_icon) return 0;
	if (argv[1]) {
		Guarkplaylist_addInto(argv[1]);
		Guarkplaylist_Read();
		Guark_data.playlistpos=Guark_data.inplaylist-1;
	}
	GuarkState ret = Createmenu();
	Guark_data.state = Sound_Play();

	//Guarkplaylist_addInto("http://uk2.mrgigabit.com:8000");


	gtk_main();
	Guark_data.state = Sound_Deinit();
  return 0;
}

// vim: noai:ts=2:sw=2
