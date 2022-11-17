#ifndef _config_struct_h_
#define _config_struct_h_

#include <stdio.h>
#include <stdlib.h>
#include <string>


#define MAXBUF 2048
#define DELIM "="

const char*  FILENAME = "rtsp.conf";
struct Config
{
    std::string rtsp_ca_cert;
    std::string rtsp_cert_pem;
    std::string rtsp_cert_key;
    std::string rtsp_server_port;
    std::string rtsp_server_mount_point;
    std::string rtsp_server_username;
    std::string rtsp_server_password;
};

Config get_config(const char *filename)
{
    Config res;
    FILE *file = fopen (filename, "r");

    if (file != NULL)
    {
        char line[MAXBUF];
        int i = 0;

        while(fgets(line, sizeof(line), file) != NULL)
        {
            line[strcspn(line, "\r\n")] = 0;
            char *cfline;
            cfline = strstr((char *)line,DELIM);
            cfline = cfline + strlen(DELIM);

            if (i == 0) {
                res.rtsp_ca_cert = {cfline};
            } else if (i == 1) {
                res.rtsp_cert_pem = {cfline};
            } else if (i == 2) {
                res.rtsp_cert_key = {cfline};
            } else if (i == 3) {
                res.rtsp_server_port = {cfline};
            } else if (i == 4) {
                res.rtsp_server_mount_point = {cfline};
            } else if (i == 5) {
                res.rtsp_server_username = {cfline};
            } else if (i == 6) {
                res.rtsp_server_password = {cfline};
            }
            i++;
        } 
        fclose(file);
    } 
    return res;
}
#endif
