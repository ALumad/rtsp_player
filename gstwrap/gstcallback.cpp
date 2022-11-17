#include "gstcallback.h"


gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *) data;
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("Stream Ends\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;
        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);
        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);
        g_main_loop_quit (loop);
        break;
    }
    default:
        break;
    }
    return TRUE;
}




void on_pad_added (GstElement *element, GstPad *pad, gpointer data){
    GstElement *decoder = (GstElement *) data;
 
    g_print ("Dynamic pad created, linking source/demuxer\n");
    GstPad *sinkpad = gst_element_get_static_pad (decoder, "sink");

    if (gst_pad_is_linked (sinkpad)) {
        g_print("*** We are already linked ***\n");
        gst_object_unref (sinkpad);
        return;
    } else {
        g_print("proceeding to linking ...\n");
    }
    GstPadLinkReturn ret = gst_pad_link (pad, sinkpad);

    if (GST_PAD_LINK_FAILED (ret)) {
        g_print("failed to link dynamically\n");
    } else {
        g_print("dynamically link successful\n");
    }

    gst_object_unref (sinkpad);
}

void on_pad_removed (GstElement *element, GstPad *pad, gpointer data){
    GstElement *decoder = (GstElement *) data;

    g_print ("Dynamic pad created, unlinking source/demuxer\n");
    GstPad *sinkpad = gst_element_get_static_pad (decoder, "sink");

    if (gst_pad_is_linked (sinkpad)) {
        g_print("proceeding to unlinking ...\n");
    } else {
        g_print("*** We are already unlinked ***\n");
        gst_object_unref (sinkpad);
        return;
    }
    gboolean ret = gst_pad_unlink (pad, sinkpad);

    if (GST_PAD_LINK_FAILED (ret)) {
        g_print("failed to unlink dynamically\n");
    } else {
        g_print("dynamically unlink successful\n");
    }
    gst_object_unref (sinkpad);
}
