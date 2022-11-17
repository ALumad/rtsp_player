#pragma once
#include <gst/gst.h>
#include <string>

class Pipeline {
public:
    Pipeline();
    ~Pipeline();
    void SetVideoSource(const char* source, 
                        const char* user,
                        const char* pass
                        );

    void SetCert(const char* pem, const char* key, const char* cacert);

    void CheckAndLink();
    void LinkPads();

    void Play();
    void Stop();
private:
    void initialize();
    GMainLoop *_loop;
    
    GstElement *_pipeline;
    GstElement *_rtspsrc;
    GstElement *_rtph264depay;
    GstElement *_h264parse;
    GstElement *_avdec_h264;
    GstElement *_videoqueue0;
    GstElement *_videoconvert;
    GstElement *_rtppcmadepay;
    GstElement *_alawdec;
    GstElement *_audioqueue0;
    GstElement *_audioconvert;

    GstElement *_video_sink;
    GstElement *_audio_sink;


};