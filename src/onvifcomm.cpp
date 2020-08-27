#include "onvifcomm.h"

OnvifComm::OnvifComm(){};
OnvifComm::~OnvifComm(){};
void OnvifComm::make_uri_withauth(char* src_uri, char* username, char* password, char* uri_auth){
    char* p = strstr(src_uri, "//");
    p += 2;
    memcpy(uri_auth, src_uri, p - src_uri);
    sprintf(uri_auth + strlen(uri_auth), "%s:%s@", username, password);
    strcat(uri_auth, p);
}

int OnvifComm::open_rtsp(char* uri_auth){
    int re = 0;
    ofstream outfile;
    av_register_all();
    avformat_network_init();
    AVFormatContext* pFormatCtx=NULL;
    AVPacket* packet = av_packet_alloc();
	
    AVDictionary* options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "2000000", 0);
    avformat_open_input(&pFormatCtx, uri_auth, NULL, NULL);
    avformat_find_stream_info(pFormatCtx, NULL);
    av_dump_format(pFormatCtx, 0, uri_auth, 0);
    outfile.open("vedio.dat", ios::out | ios::trunc );
    
    for(;;){
	re = av_read_frame(pFormatCtx, packet); 
        if (re != 0){
            return re;
	}
        outfile << packet << endl;
        av_packet_unref(packet);	    
    }
    avformat_close_input(&pFormatCtx);
    outfile.close();
    return re;        
}
