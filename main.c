#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gdk/gdk.h>


#include "variables.h"
#include "notify.c"
#include "gst.c"
#include "gtk.c"
#include "playlist.c"


int main (int argc, char *argv[]) {
	int i;

	if (proc_find("guark")==2) { // Если пытаемся запустить еще 1 гварк
		if (argv[1]) i = Guarkplaylist_addInto(argv[1]); // Добавляем файл argv[1] в плейлист
		if (i) return 0;
		return 0;
	}

	gst_init (&argc, &argv);
	remove("/tmp/guark.status");
	loop = g_main_loop_new (NULL, FALSE);
	if (!(tray_icon = Guark_Init(argc, &argv[0]))) return 0;
	Guark_data.playlistpos = Guark_data.inplaylist-1; // Start last track

	if (argv[1]) {
		i = Guarkplaylist_addInto(argv[1]);
		if (i) { printf("Выбран неверный файл для воспроизведения\n"); return 0; }
	}
	else {
		Guark_data.pipeline = Sound_init(Guark_playlist[Guark_data.playlistpos].track);
		Guark_data.state = Sound_Play();
	}

	GuarkState ret = Createmenu();

	//Guarkplaylist_addInto("http://uk2.mrgigabit.com:8000");
	gtk_main();
	Guark_data.state = Sound_Deinit();
  return 0;
}

// vim: noai:ts=2:sw=2
