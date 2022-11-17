#pragma once
#include <gst/gst.h>
gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer data);
void on_pad_added (GstElement *element, GstPad *pad, gpointer data);
void on_pad_removed (GstElement *element, GstPad *pad, gpointer data);