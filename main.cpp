#include "onvifdevice.h"
#include "onvifcomm.h"
#include <unistd.h>

int main(){ 
    vector<string> addr;
    detectDevice(addr);
    OnvifComm comm;
    for(vector<string>::iterator it = addr.begin(); it != addr.end(); ++it){
        //cout<<"addr: "<<it<<endl;
        OnvifDevice onvifDevice(*it, "admin", "leinao123");
        onvifDevice.ptzContinuousMove(LEFT);
        onvifDevice.ptzContinuousStop();
        //string mediaAddr;
        //onvifDevice.getMediaUrl(mediaAddr);
        //cout<<"mediaAddr: "<<mediaAddr<<endl;
        /*
        string profileToken;
        onvifDevice.getProfile(profileToken);
        cout<<"profileToken: "<<profileToken<<endl;
        string rtspUrl;
        onvifDevice.getRTSPUrl(rtspUrl);
        cout<<"rtspUrl: "<<rtspUrl<<endl;

	if(rtspUrl.empty()){
             continue;
        }
        */
	//vector<string> PresetToken;
	//onvifDevice.ptzPresetTour(PresetToken);
	//cout<<"PresetToken: "<<PresetToken[10]<<end;
	
        //onvifDevice.ptzPreset(SET,"4");
        //onvifDevice.ptzContinuousMove(DOWN);
        //onvifDevice.ptzContinuousStop();
	
	//onvifDevice.ptzPreset(GOTO,"3");
	//sleep(5);
	//onvifDevice.ptzPreset(REMOVE,"10");
        /*
        string imageUrl;
	//char uri_auth[200]={0};
        //onvifDevice.getIMAGEUrl(imageUrl);
	//cout<<"imageUrl: "<<imageUrl<<endl;
        //comm.make_uri_withauth((char *)(imageUrl).c_str(), "admin", "123456", uri_auth);
        //cout<<"uri_auth: "<<uri_auth<<endl;
	
        //char uri_auth[200]={0};
        //comm.make_uri_withauth((char *)(rtspUrl).c_str(), "admin", "leinao123", uri_auth);
        //cout<<"uri_auth: "<<uri_auth<<endl;
        //comm.open_rtsp(uri_auth); 
        string PTZUrl;
        onvifDevice.getPTZUrl(PTZUrl);
        cout<<"PTZUrl: "<<PTZUrl<<endl;
        string Manufacturer,Model,FirmwareVersion,SerialNumber,HardwareId;
        onvifDevice.getDeviceInformation(Manufacturer, Model,FirmwareVersion,
                               SerialNumber,HardwareId);
        cout<<"Manufacturer: "<<Manufacturer<<" Model: "<<Model<<endl;
        cout<<"HardwareId: "<<HardwareId<<" SerialNumber: "<<Model<<endl;
        onvifDevice.setDateTime();
        int year,month,day,hour,minute,second;
        onvifDevice.getDateTime(year, month, day, hour, minute, second);
        cout<<year<<"-"<<month<<"-"<<day<<endl;
        cout<<hour<<":"<<minute<<":"<<second<<endl;
        onvifDevice.ptzRelativeMove(LEFT);
	*/
    }
    return 0;
}
