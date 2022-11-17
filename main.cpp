//Usage
//First run
// ./rtsp_server
//Then run
//./rtsp_client rtsps://127.0.0.1:8554/test

//Display RTSP streaming of video
//(c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>

#include <string.h>
#include <math.h>
#include <gst/gst.h>
#include <glib.h>
#include "config.h"
#include "gstwrap/pipeline.h"



int main (int argc, char *argv[])
{
   
    if(argc!=2){
      g_print("Please pass the URL of the RTSP stream\ne.g. %s rtsps://127.0.0.1:8554/test\n",argv[0]);
      exit(1);
    }


    Config config = get_config(FILENAME);

    g_print("%s\n",config.rtsp_ca_cert.data());
    g_print("%s\n",config.rtsp_cert_pem.data());
    g_print("%s\n",config.rtsp_cert_key.data());
    g_print("%s\n",config.rtsp_server_port.data());
    g_print("%s\n",config.rtsp_server_mount_point.data());
    g_print("%s\n",config.rtsp_server_username.data());
    g_print("%s\n",config.rtsp_server_password.data());

    gst_init (&argc, &argv);
     

    Pipeline pipeline;
    pipeline.SetVideoSource(argv[1], config.rtsp_server_username.data(), config.rtsp_server_password.data());
    pipeline.SetCert(config.rtsp_cert_pem.data(), config.rtsp_cert_key.data(), config.rtsp_ca_cert.data());
    pipeline.CheckAndLink();
    pipeline.LinkPads();

    g_print ("Playing: %s\n", argv[1]);
    pipeline.Play(); //until !eof

    pipeline.Stop();

    return 0;
}
