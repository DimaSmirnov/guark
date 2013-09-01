guark: main.c gst.c gtk.c variables.h
	gcc main.c `pkg-config --cflags --libs gstreamer-0.10` `pkg-config --cflags --libs gtk+-2.0` -o guark -std=c99

install: guark
	cp guark /usr/bin/guark
	mkdir /usr/share/pixmaps/guark/
	cp icons/guark.png /usr/share/pixmaps/guark/guark.png
	cp icons/rewind.png /usr/share/pixmaps/guark/rewind.png
	cp icons/forward.png /usr/share/pixmaps/guark/forward.png
	cp icons/stop.png /usr/share/pixmaps/guark/stop.png
	cp icons/play.png /usr/share/pixmaps/guark/play.png

uninstall: guark
	rm /usr/bin/guark
	rm -r /usr/share/pixmaps/guark/
