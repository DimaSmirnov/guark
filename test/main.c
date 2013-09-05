#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <gst/gst.h>

GstTagList *tags;


static void print_tags() {
  gchar *artist = "Artist";
  gchar *title = "Music";

  /* If already known, get the Artist and Title tags of the media  */
  gst_tag_list_get_string(tags, GST_TAG_ARTIST, &artist);
  gst_tag_list_get_string(tags, GST_TAG_TITLE, &title);
  g_print("%s - %s :", artist, title);
}

static gboolean cb_print_position (GstElement *pipeline) {
  GstFormat fmt = GST_FORMAT_TIME;
  gint64 pos, len;

  print_tags();

  if (gst_element_query_position (pipeline, &fmt, &pos)
    && gst_element_query_duration (pipeline, &fmt, &len)) {
      g_print (" %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
               GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
  }

  /* call me again */
  return TRUE;
}

static gboolean bus_call (GstBus     *bus,GstMessage *msg,gpointer    data) {
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("\nEnd of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Bus error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }

    case GST_MESSAGE_TAG: {
      GstTagList *newTags = gst_tag_list_new();
      gst_message_parse_tag(msg, &newTags);
      tags = gst_tag_list_merge(tags, newTags, GST_TAG_MERGE_PREPEND);

      gst_tag_list_free(newTags);

      break;
    }

    default:
      break;
  }

  return TRUE;
}

static void add_bus_watch(GstElement *pipeline,GMainLoop *loop) {
  GstBus *bus;
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);
}

static GstElement *init_pipeline(GMainLoop *loop) {
  GstElement *playbin;

  /* Create gstreamer elements */
  playbin  = gst_element_factory_make ("playbin2", "play");

  if (!playbin) {
    g_printerr ("One element could not be created. Exiting.\n");
    return NULL;
  }

  add_bus_watch(playbin, loop);

  return playbin;
}

gboolean set_uri(GstElement *playbin, const char *uri) {
  /* Set the input filename to the source element */
  GError *error = NULL;
  g_object_set (G_OBJECT (playbin), "uri",
	gst_filename_to_uri(uri, &error), NULL);

  if (error) {
		g_print("Filename to URI error: %s\n", error->message);
		g_error_free(error);
	return FALSE;
  }
return TRUE;
}

gint main (int   argc, char *argv[]) {
  GMainLoop *loop;
  GstElement *pipeline;
  gint i;

  gst_init (&argc, &argv); //

  if (argc < 2) g_error ("Usage: %s   ...", argv[0]);

  loop = g_main_loop_new (NULL, FALSE);//
  pipeline = init_pipeline(loop);//

  if (!pipeline) return -1;

  g_timeout_add (200, (GSourceFunc) cb_print_position, pipeline);

  for (i = 1; i < argc; i++) {
    if (!set_uri(pipeline, argv[i])) {
        continue;
    }

    tags = gst_tag_list_new();

    /* Start playing */
    g_print ("Running...\n");
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run (loop);

    /* Out of the main loop, clean up nicely */
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_tag_list_free(tags);

    /* Has the execution ended? */
    g_print("\n");
    if (i < argc - 1) {
        g_print("Next track...\n\n");
    } else {
        g_print ("Deleting pipeline\n");
        gst_object_unref (GST_OBJECT (pipeline));
        g_print("End of execution!\n");
    }
  }

  return 0;
}
