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

	if (proc_find("guark")==2) return 1; // Если пытаемся запустить еще 1 гварк

	Guark_data.state = Sound_init(argc, &argv[0]);
	tray_icon = Guark_Init(argc, &argv[0]);
	Guarkplaylist_Init();
	GuarkState ret = Createmenu();


	if (argv[1]) Guarkplaylist_addInto(argv[1]);
	else Guarkplaylist_addInto("http://uk2.mrgigabit.com:8000");


	Guark_data.state = Sound_Play();
	gtk_main();
	Guark_data.state = Sound_Deinit();
  return 0;
}
