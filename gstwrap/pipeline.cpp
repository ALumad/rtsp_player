#include <glib.h>
#include <gio/gio.h>
#include "pipeline.h"
#include "gstcallback.h"
#include "client.h"

Pipeline::Pipeline(){
    _loop = g_main_loop_new (NULL, FALSE);
    initialize();
}


Pipeline::~Pipeline(){
    g_print ("Eliminating Pipeline\n");
    gst_object_unref (GST_OBJECT (_pipeline));   
}


void Pipeline::Play(){
    gst_element_set_state (_pipeline, GST_STATE_PLAYING);
    g_main_loop_run (_loop);    
}
void Pipeline::Stop(){
    g_print ("End of the Streaming... ending the playback\n");
    gst_element_set_state (_pipeline, GST_STATE_NULL);
}

void Pipeline::initialize(){

    _pipeline = gst_pipeline_new ("video player");
    g_assert (_pipeline);

    _rtspsrc = gst_element_factory_make ("rtspsrc", "rtspsrc0");
    g_assert (_rtspsrc);
    _rtph264depay = gst_element_factory_make ("rtph264depay", "rtph264depay0");
    g_assert (_rtph264depay);
    _h264parse = gst_element_factory_make ("h264parse", "h264parse0");
    g_assert (_h264parse);
    _avdec_h264 = gst_element_factory_make ("decodebin", "avdec_h2640");
    g_assert (_avdec_h264);
    _videoqueue0 = gst_element_factory_make ("queue", "videoqueue0");
    g_assert (_videoqueue0);
    _videoconvert = gst_element_factory_make ("videoconvert", "videoconvert0");
    g_assert (_videoconvert);
    
    _video_sink = gst_element_factory_make ("ximagesink", "autovideosink0");
    g_assert (_video_sink);
    g_object_set (G_OBJECT (_video_sink), "sync", FALSE, NULL);
    
    _rtppcmadepay = gst_element_factory_make ("rtppcmadepay",  "rtppcmadepay0");
    g_assert(_rtppcmadepay);
    _alawdec = gst_element_factory_make ("decodebin", "alawdec0");
    g_assert (_alawdec);
    _audioqueue0 = gst_element_factory_make ("queue", "audioqueue0");
    g_assert (_audioqueue0);
    _audioconvert = gst_element_factory_make ("audioconvert", "audioconvert0");
    g_assert (_audioconvert);

    _audio_sink = gst_element_factory_make ("autoaudiosink", "autoaudiosink0");
    g_assert (_audio_sink);
    g_object_set (G_OBJECT (_audio_sink), "sync", FALSE, NULL);

}

void Pipeline::SetVideoSource(const char* source, const char* user, const char* pass)
{
    g_object_set (G_OBJECT(_rtspsrc), "location", source, NULL);
    g_object_set (G_OBJECT(_rtspsrc), "do-rtcp", TRUE, NULL);
    g_object_set (G_OBJECT(_rtspsrc), "latency", 0, NULL);
    g_object_set (G_OBJECT(_rtspsrc), "user-id", user, NULL);
    g_object_set (G_OBJECT(_rtspsrc), "user-pw", pass, NULL);    

}

void Pipeline::SetCert(const char* pem, const char* key, const char*cacert){
    GError *error=NULL;
    g_object_set (G_OBJECT (_rtspsrc), "tls-validation-flags", G_TLS_CERTIFICATE_VALIDATE_ALL, NULL);    
    GTlsCertificate *cert;
    cert = g_tls_certificate_new_from_files(pem,key,&error);

    if (cert == NULL) {
        g_printerr ("failed to parse PEM: %s\n", error->message);
        exit(-1);
    }
    GTlsDatabase* database;
    database = g_tls_file_database_new (cacert, &error);
    GTlsCertificate *ca_cert;
    ca_cert = g_tls_certificate_new_from_file(cacert,&error);

    if (ca_cert == NULL) {
        g_printerr ("failed to parse CA PEM: %s\n", error->message);
        exit(-1);
    }

    g_object_set (G_OBJECT(_rtspsrc), "tls-database", database, NULL);
    RtspClientTlsInteraction *interaction;
    interaction = rtsp_client_tls_interaction_new (cert, ca_cert, database);

    g_object_set (G_OBJECT(_rtspsrc), "tls-interaction", interaction, NULL);

}





void Pipeline::CheckAndLink(){
    GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (_pipeline));
    gst_bus_add_watch (bus, bus_call, _loop);
    gst_object_unref (bus);

    if (!_pipeline || !_rtspsrc || !_rtph264depay || !_h264parse 
        || !_avdec_h264 || !_videoqueue0 || !_videoconvert || !_rtppcmadepay 
        || !_alawdec || !_audioqueue0 || !_audioconvert || !_audio_sink) 
    {
        g_printerr ("One of the elements wasn't created... Exiting\n");
        exit(-1);
    }

    gst_bin_add_many (GST_BIN (_pipeline), _rtspsrc, _rtph264depay, _h264parse, 
            _avdec_h264, _videoqueue0, _videoconvert, _video_sink, _rtppcmadepay, _alawdec, 
            _audioqueue0, _audioconvert, _audio_sink, NULL);

    if (!gst_element_link_many (_rtph264depay, _h264parse, _avdec_h264, NULL)){
        g_warning ("Linking _rtspsrc.._audio_sink Failed");
        exit(-2);
    }

    if (!gst_element_link_many (_rtppcmadepay, _alawdec, NULL)){
        g_warning ("Linking _rtppcmadepay->_alawdec Failed");
        exit(-3);
    }
    
    if (!gst_element_link_many (_videoqueue0, _videoconvert, _video_sink, NULL)){
        g_warning ("Linking _videoqueue0.._video_sink Failed...");
        exit(-4);
    }
    
    if (!gst_element_link_many (_audioqueue0, _audioconvert, _audio_sink, NULL)){
        g_warning ("Linking _audioqueue0.._audiosink Failed");
        exit(-5);

    }


}

void Pipeline::LinkPads(){
    if(! g_signal_connect (_rtspsrc, "pad-added", G_CALLBACK (on_pad_added),_rtph264depay))
    {
        g_warning ("Linking part (1) with part (A)-1 Fail...");
    }

    if(! g_signal_connect (_avdec_h264, "pad-added", G_CALLBACK (on_pad_added),_videoqueue0))
    {
        g_warning ("Linking part (2) with part (A)-2 Fail...");
    }

    if(! g_signal_connect (_rtspsrc, "pad-added", G_CALLBACK (on_pad_added),_rtppcmadepay))
    {
        g_warning ("Linking part (1) with part (B)-1 Fail...");
    }

    if(! g_signal_connect (_alawdec, "pad-added", G_CALLBACK (on_pad_added),_audioqueue0))
    {
        g_warning ("Linking part (2) with part (B)-2 Fail...");
    }
}