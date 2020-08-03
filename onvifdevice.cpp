#include "onvifdevice.h"
#include "stdio.h"
#include "wsdd.nsmap"
#include "wsseapi.h"
#include "wsaapi.h"
#include "ErrorLog.h"
#include "soapDeviceBindingProxy.h"
#include "soapMediaBindingProxy.h"
#include "soapPTZBindingProxy.h"
#include "soapPullPointSubscriptionBindingProxy.h"
#include "soapRemoteDiscoveryBindingProxy.h" 

OnvifDevice::OnvifDevice(string url,string username,string passwd):m_deviceurl(url),m_username(username),m_passwd(passwd){}
OnvifDevice::~OnvifDevice(){}

/************************************************************************
**函数：ptzPresetTour
**功能：获取Preset Options
************************************************************************/
int OnvifDevice::ptzPresetTour(vector<string> &PresetToken){
    int result = 0;
    std::string strProfileToken;
    result = getProfile(strProfileToken);
    if (result!=SOAP_OK){
         return -1;
    }
    std::string PTZAddr;
    result = getPTZUrl(PTZAddr);
    if (result!=SOAP_OK){
         return -1;
    }    
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
         return -1;
    }
    _tptz__GetPresetTourOptions         preset;
    _tptz__GetPresetTourOptionsResponse response;
    preset.ProfileToken = strProfileToken;
    result = proxyPTZ.GetPresetTourOptions(proxyPTZ.soap_endpoint,NULL,&preset, &response);
    if (SOAP_OK != result){
        return -1;
    }
    PresetToken = response.Options->TourSpot->PresetDetail->PresetToken;
    return SOAP_OK;
}

/************************************************************************
**函数：ptzPreset
**功能：控制预置位
************************************************************************/
int OnvifDevice::ptzPreset(int command, string presetToken){
    int result = 0;
    std::string strProfileToken;
    result = getProfile(strProfileToken);
    if (result!=SOAP_OK){
         return -1;
    }
    std::string PTZAddr;
    result = getPTZUrl(PTZAddr);
    if (result!=SOAP_OK){
         return -1;
    }    
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
         return -1;
    }
    if (command == GOTO){
        _tptz__GotoPreset preset;
        _tptz__GotoPresetResponse response;
        preset.ProfileToken = strProfileToken;
        preset.PresetToken = presetToken;
        preset.Speed = soap_new_tt__PTZSpeed(proxyPTZ.soap, -1);
        preset.Speed->PanTilt = soap_new_tt__Vector2D(proxyPTZ.soap, -1);
        std::string str="http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace";
        preset.Speed->PanTilt->space =&str;
        preset.Speed->Zoom = soap_new_tt__Vector1D(proxyPTZ.soap, -1);
        //preset.Speed->PanTilt->x = -((float)speed / 1);
        //preset.Speed->PanTilt->y = 0;

        result = proxyPTZ.GotoPreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
        if (SOAP_OK != result){
            cout<<"GOTO FAIL"<<endl;
            return -1;
        }
        cout<<"GOTO SUCC"<<endl;
    }else if (command == SET){
        _tptz__SetPreset preset;
        _tptz__SetPresetResponse response;
        preset.ProfileToken = strProfileToken;
        preset.PresetToken = &presetToken;
        result = proxyPTZ.SetPreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
        if (SOAP_OK != result){
             cout<<"SET FAIL"<<endl;
             return -1;
        }
        cout<<"SET SUCC"<<endl;
    }else if (command == REMOVE){
        _tptz__RemovePreset preset;
        _tptz__RemovePresetResponse response;
        preset.ProfileToken = strProfileToken;
        preset.PresetToken = presetToken;
        result = proxyPTZ.RemovePreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
        if (SOAP_OK != result){
             cout<<"REMOVE FAIL"<<endl;
             return -1;
         }
         cout<<"REMOVE SUCC"<<endl;
    }
    return SOAP_OK;
}

/************************************************************************
**函数：ptzRelativeMove
**功能：相机控制
************************************************************************/
int OnvifDevice::ptzRelativeMove(int command,int speed){
    int result = 0;
    std::string strProfileToken;
    result = getProfile(strProfileToken);
    if (result!=SOAP_OK){
         return -1;
    }
    std::string PTZAddr;
    result = getPTZUrl(PTZAddr);
    if (result!=SOAP_OK){
         return -1;
    }    
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
         return -1;
    }
    _tptz__RelativeMove ptz_req;
    _tptz__RelativeMoveResponse ptz_rep;
    ptz_req.ProfileToken = strProfileToken;
    ptz_req.Speed = soap_new_tt__PTZSpeed(proxyPTZ.soap, -1);

    ptz_req.Speed->PanTilt = (struct tt__Vector2D *)soap_new_tt__Vector2D(proxyPTZ.soap, -1);
    ptz_req.Translation = soap_new_tt__PTZVector(proxyPTZ.soap, -1);
    ptz_req.Translation->PanTilt = (struct tt__Vector2D *)soap_new_tt__Vector2D(proxyPTZ.soap, -1);
    tt__Vector1D* zoom = (struct tt__Vector1D *)soap_new_tt__Vector1D(proxyPTZ.soap, -1);
    ptz_req.Translation->Zoom = zoom;
    std::string str="http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace";
    ptz_req.Translation->PanTilt->space =&str;
    std::string str2 ="http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace";
    ptz_req.Translation->Zoom->space = &str2;

    switch (command){
    case LEFT:
        ptz_req.Translation->PanTilt->x = -((float)speed / 5);
        ptz_req.Translation->PanTilt->y = 0;
        break;
    case RIGHT:
        ptz_req.Translation->PanTilt->x = ((float)speed / 5);
        ptz_req.Translation->PanTilt->y = 0;
        break;
    case UP:
        ptz_req.Translation->PanTilt->x = 0;
        ptz_req.Translation->PanTilt->y = ((float)speed / 5);
        break;
    case DOWN:
        ptz_req.Translation->PanTilt->x = 0;
        ptz_req.Translation->PanTilt->y = -((float)speed / 5);
        break;
    case LEFTUP:
    	ptz_req.Translation->PanTilt->x  = -((float)speed / 5);
    	ptz_req.Translation->PanTilt->y = ((float)speed / 5);
    	break;
    case LEFTDOWN:
    	ptz_req.Translation->PanTilt->x  = -((float)speed / 5);
    	ptz_req.Translation->PanTilt->y = -((float)speed / 5);
    	break;
    case RIGHTUP:
    	ptz_req.Translation->PanTilt->x  = ((float)speed / 5);
    	ptz_req.Translation->PanTilt->y = ((float)speed / 5);
    	break;
    case RIGHTDOWN:
    	ptz_req.Translation->PanTilt->x  = ((float)speed / 5);
    	ptz_req.Translation->PanTilt->y = -((float)speed / 5);
    	break;
    case ZOOMIN:
        ptz_req.Translation->Zoom->x = ((float)speed / 5);
        break;
    case ZOOMOUT:
        ptz_req.Translation->Zoom->x = -((float)speed / 5);
        break;
    default:
        break;
    }
    result = proxyPTZ.RelativeMove(proxyPTZ.soap_endpoint,NULL,&ptz_req, &ptz_rep);
    if (SOAP_OK != result){
      cout<<"##fail##"<<endl;
      return -1;
    }
    cout<<"##succ##"<<endl;
    return SOAP_OK;
}

/************************************************************************
**函数：ptzContinuousMove
**功能：持续控制
************************************************************************/
int OnvifDevice::ptzContinuousMove(int command,int speed){
    int result = 0;
    std::string strProfileToken;
    result = getProfile(strProfileToken);
    if (result != SOAP_OK){
        return -1;
    }
    std::string PTZAddr;
    result = getPTZUrl(PTZAddr);
    if (result!=SOAP_OK){
         return -1;
    }   
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL,  m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    _tptz__ContinuousMove continuousMove;
    _tptz__ContinuousMoveResponse response;
    continuousMove.ProfileToken = const_cast<char *>(strProfileToken.c_str());
    continuousMove.Velocity = soap_new_tt__PTZSpeed(proxyPTZ.soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(proxyPTZ.soap, -1);
    std::string str="http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace";
    continuousMove.Velocity->PanTilt->space =&str;  
    continuousMove.Velocity->Zoom = soap_new_tt__Vector1D(proxyPTZ.soap, -1);

    switch (command){
    case LEFT:
    	continuousMove.Velocity->PanTilt->x = -((float)speed / 5);
    	continuousMove.Velocity->PanTilt->y = 0;
    	break;
    case RIGHT:
    	continuousMove.Velocity->PanTilt->x = ((float)speed / 5);
    	continuousMove.Velocity->PanTilt->y = 0;
    	break;
    case UP:
    	continuousMove.Velocity->PanTilt->x = 0;
    	continuousMove.Velocity->PanTilt->y = ((float)speed / 5);
    	break;
    case DOWN:
    	continuousMove.Velocity->PanTilt->x = 0;
    	continuousMove.Velocity->PanTilt->y = -((float)speed / 5);
    	break;
    case LEFTUP:
    	continuousMove.Velocity->PanTilt->x = -((float)speed / 5);
    	continuousMove.Velocity->PanTilt->y = ((float)speed / 5);
    	break;
    case LEFTDOWN:
    	continuousMove.Velocity->PanTilt->x = -((float)speed / 5);
    	continuousMove.Velocity->PanTilt->y = -((float)speed / 5);
    	break;
    case RIGHTUP:
    	continuousMove.Velocity->PanTilt->x = ((float)speed / 5);
    	continuousMove.Velocity->PanTilt->y = ((float)speed / 5);
    	break;
    case RIGHTDOWN:
    	continuousMove.Velocity->PanTilt->x = ((float)speed / 5);
    	continuousMove.Velocity->PanTilt->y = -((float)speed / 5);
    	break;
    case ZOOMIN:
    	continuousMove.Velocity->Zoom->x = ((float)speed / 5);
    	break;
    case ZOOMOUT:
    	continuousMove.Velocity->Zoom->x = -((float)speed / 5);
    	break;
    default:
    	break;
    }
    result = proxyPTZ.ContinuousMove(proxyPTZ.soap_endpoint,NULL,&continuousMove, &response);
    if (SOAP_OK != result){
      cout<<"##fail##"<<endl;
      return -1;
    }
    cout<<"##succ##"<<endl;
    return SOAP_OK;
}
/************************************************************************
**函数：ptzContinuousStop
**功能：相机控制停止
************************************************************************/
int OnvifDevice::ptzContinuousStop(){
    int result = 0;
    std::string strProfileToken;
    result = getProfile(strProfileToken);
    if (result!=SOAP_OK){
		return -1;
    }
    std::string PTZAddr;
    result = getPTZUrl(PTZAddr);
    if (result!=SOAP_OK){
         return -1;
    }   
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL,  m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    _tptz__Stop stopImmediate;
    _tptz__StopResponse response;
    stopImmediate.ProfileToken = const_cast<char *>(strProfileToken.c_str());      
    result = proxyPTZ.Stop(proxyPTZ.soap_endpoint,NULL,&stopImmediate, &response);
    if (SOAP_OK != result){
      cout<<"##fail##"<<endl;
      return -1;
    }
    cout<<"##succ##"<<endl;
    return SOAP_OK;    
}
/************************************************************************
**函数：getRTSPUrl
**功能：获取RTSP地址
************************************************************************/
int OnvifDevice::getRTSPUrl(string& rtspUrl){
    int result = 0;
    std::string mediaAddr;
    result = getMediaUrl(mediaAddr);
    if (SOAP_OK != result){	
          return -1;
    }
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
	  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    tt__Transport               transport;
    transport.Tunnel = NULL;
    transport.Protocol = tt__TransportProtocol__RTSP;
    tt__StreamSetup             setup;
    setup.Transport = &transport;
    setup.Stream = tt__StreamType__RTP_Unicast;
    _trt__GetStreamUri          StreamUri_req;
    _trt__GetStreamUriResponse  StreamUri_rep;
    StreamUri_req.StreamSetup = &setup;
    string profileToken;
    getProfile(profileToken);
    StreamUri_req.ProfileToken = profileToken;
    result = proxyMedia.GetStreamUri(proxyMedia.soap_endpoint, NULL, &StreamUri_req, &StreamUri_rep);
    if (SOAP_OK != result){
        return -1;
    }
    rtspUrl = StreamUri_rep.MediaUri->Uri;
    return SOAP_OK;
}
/************************************************************************
**函数：getIMAGEUrl
**功能：获取图像抓拍地址
************************************************************************/
int OnvifDevice::getIMAGEUrl(string& imageUrl){
    int result = 0;
    std::string mediaAddr;
    result = getMediaUrl(mediaAddr);
    if (SOAP_OK != result){	
          return -1;
    }
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
	  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
	  }
    _trt__GetSnapshotUri         ImageUri_req; 
    _trt__GetSnapshotUriResponse ImageUri_rep;
    string profileToken;
    getProfile(profileToken);
    ImageUri_req.ProfileToken = profileToken;
    result = proxyMedia.GetSnapshotUri(proxyMedia.soap_endpoint, NULL, &ImageUri_req, &ImageUri_rep);
    if (SOAP_OK != result){
        return -1;
    }
    imageUrl = ImageUri_rep.MediaUri->Uri;
    return SOAP_OK;
}
/************************************************************************
**函数：getProfile
**功能：获取profileToken
************************************************************************/
int OnvifDevice::getProfile(string& profileToken){
    int result = 0;
    std::string mediaAddr;
    result = getMediaUrl(mediaAddr);
    if (SOAP_OK != result){	
          return -1;
    }
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    _trt__GetProfiles           Profiles_req;
    //存储的是获取回来的信息
    _trt__GetProfilesResponse   Profiles_rep;
    result = proxyMedia.GetProfiles(proxyMedia.soap_endpoint, NULL, &Profiles_req, &Profiles_rep);
    if (SOAP_OK != result){	
          return -1;
    }
    ///< 可能会有好几路流，相应的也会有好几个profile,这里只取第一路码流
    profileToken = Profiles_rep.Profiles[0]->token;
    return SOAP_OK;
}
/************************************************************************
**函数：getDateTime
**功能：获取设备时间
************************************************************************/
int OnvifDevice::getDateTime(int& year, int& month, int& day,
                             int& hour, int& minute, int& second){
    DeviceBindingProxy proxyDevice;
    proxyDevice.soap_endpoint = m_deviceurl.c_str();
	  soap_register_plugin(proxyDevice.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
	  }
    _tds__GetSystemDateAndTime         GetTm_req;
    _tds__GetSystemDateAndTimeResponse GetTm_rep;
    int result = proxyDevice.GetSystemDateAndTime(proxyDevice.soap_endpoint, NULL, &GetTm_req, &GetTm_rep);
    if (SOAP_OK != result){	
          return -1;
    }
    year = GetTm_rep.SystemDateAndTime->UTCDateTime->Date->Year;
    month = GetTm_rep.SystemDateAndTime->UTCDateTime->Date->Month;
    day = GetTm_rep.SystemDateAndTime->UTCDateTime->Date->Day;
    hour = GetTm_rep.SystemDateAndTime->UTCDateTime->Time->Hour;
    minute = GetTm_rep.SystemDateAndTime->UTCDateTime->Time->Minute;
    second = GetTm_rep.SystemDateAndTime->UTCDateTime->Time->Second;
    return SOAP_OK;
}
/************************************************************************
**函数：setDateTime
**功能：设置设备时间
************************************************************************/
int OnvifDevice::setDateTime(){
    DeviceBindingProxy proxyDevice;
    proxyDevice.soap_endpoint = m_deviceurl.c_str();
	  soap_register_plugin(proxyDevice.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
	  }
    _tds__SetSystemDateAndTime           SetTm_req;
    _tds__SetSystemDateAndTimeResponse   SetTm_rep;
    time_t t = time(NULL);     
    tm *stm = gmtime(&t);
    //给需要添加值的变量先创建空间
    SetTm_req.UTCDateTime = soap_new_tt__DateTime(proxyDevice.soap, -1);
    SetTm_req.UTCDateTime->Date = soap_new_tt__Date(proxyDevice.soap, -1);
    SetTm_req.UTCDateTime->Time = soap_new_tt__Time(proxyDevice.soap, -1);
    
    SetTm_req.UTCDateTime->Date->Year  = stm->tm_year + 1900;
    SetTm_req.UTCDateTime->Date->Month  = stm->tm_mon + 1;
    SetTm_req.UTCDateTime->Date->Day    = stm->tm_mday;
    SetTm_req.UTCDateTime->Time->Hour   = stm->tm_hour;
    SetTm_req.UTCDateTime->Time->Minute = stm->tm_min;
    SetTm_req.UTCDateTime->Time->Second = stm->tm_sec;
    int result = proxyDevice.SetSystemDateAndTime(proxyDevice.soap_endpoint, NULL, &SetTm_req, &SetTm_rep);
    if (SOAP_OK != result){	
          return -1;
    }
    return SOAP_OK;
}
/************************************************************************
**函数：getMediaUrl
**功能：获取Media地址
************************************************************************/
int OnvifDevice::getMediaUrl(string& mediaAddr){
    DeviceBindingProxy proxyDevice;
    proxyDevice.soap_endpoint = m_deviceurl.c_str();
	  soap_register_plugin(proxyDevice.soap, soap_wsse);
	  if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    _tds__GetCapabilities           Bilities_req;
    _tds__GetCapabilitiesResponse   Bilities_rep;
    int result = proxyDevice.GetCapabilities(proxyDevice.soap_endpoint, NULL, &Bilities_req, &Bilities_rep);
    if (SOAP_OK != result){
        return -1;
    }
    mediaAddr = Bilities_rep.Capabilities->Media->XAddr;
    return SOAP_OK;
}
/************************************************************************
**函数：getPTZUrl
**功能：获取PTZ地址
************************************************************************/
int OnvifDevice::getPTZUrl(string& PTZAddr){
    DeviceBindingProxy proxyDevice;
    proxyDevice.soap_endpoint = m_deviceurl.c_str();
    soap_register_plugin(proxyDevice.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    _tds__GetCapabilities           Bilities_req;
    _tds__GetCapabilitiesResponse   Bilities_rep;
    int result = proxyDevice.GetCapabilities(proxyDevice.soap_endpoint, NULL, &Bilities_req, &Bilities_rep);
    if (SOAP_OK != result){
		return -1;
    }
    PTZAddr = Bilities_rep.Capabilities->PTZ->XAddr;
    return SOAP_OK;
}
/************************************************************************
**函数：getDeviceInformation
**功能：获取设备信息
************************************************************************/
int OnvifDevice::getDeviceInformation(string& Manufacturer,string& Model,string& FirmwareVersion,string& SerialNumber,string& HardwareId){
    DeviceBindingProxy proxyDevice;
    proxyDevice.soap_endpoint = m_deviceurl.c_str();
    soap_register_plugin(proxyDevice.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, m_username.c_str(), m_passwd.c_str()))
    {
		return -1;
    }
    _tds__GetDeviceInformation           devinfo_req;
    _tds__GetDeviceInformationResponse   devinfo_resp;

    int result = proxyDevice.GetDeviceInformation(proxyDevice.soap_endpoint, NULL, &devinfo_req, &devinfo_resp);
    if (SOAP_OK != result){
        return -1;
    }
    Manufacturer = devinfo_resp.Manufacturer;
    Model = devinfo_resp.Model;
    FirmwareVersion = devinfo_resp.FirmwareVersion;
    SerialNumber = devinfo_resp.SerialNumber;
    HardwareId = devinfo_resp.HardwareId;
    return SOAP_OK;
}
/************************************************************************
**函数：detectDevice
**功能：设备搜索
************************************************************************/
void detectDevice(vector<string>& vecDevAddr){
    vector<string> s_dev;
    s_dev.clear();
    struct soap *soap = soap_new();
    soap_set_namespaces(soap, namespaces);                                 // 设置soap的namespaces
    soap->recv_timeout    = 10;                                            // 设置超时（超过指定时间没有数据就退出）
    soap->send_timeout    = 10;
    soap->connect_timeout = 10;
    soap->socket_flags = MSG_NOSIGNAL;
    soap_set_mode(soap, SOAP_C_UTFSTRING);
    struct SOAP_ENV__Header *header = NULL;
    header = (struct SOAP_ENV__Header *)soap_malloc(soap, sizeof(struct SOAP_ENV__Header));
    memset(header, 0x00 ,sizeof(struct SOAP_ENV__Header));
    soap_default_SOAP_ENV__Header(soap, header);
    header->wsa__MessageID = (char*)soap_wsa_rand_uuid(soap);
    header->wsa__To        = (char*)soap_malloc(soap, strlen(SOAP_TO) + 1);
    memset(header->wsa__To, 0, strlen(SOAP_TO) + 1);
    header->wsa__Action    = (char*)soap_malloc(soap, strlen(SOAP_ACTION) + 1);
    memset(header->wsa__Action, 0, strlen(SOAP_ACTION) + 1);
    strcpy(header->wsa__To, SOAP_TO);
    strcpy(header->wsa__Action, SOAP_ACTION);
    soap->header = header;
    struct wsdd__ProbeType  req;
    struct wsdd__ScopesType *scope = NULL;                                      // 用于描述查找哪类的Web服务
  
    scope = (struct wsdd__ScopesType *)soap_malloc(soap, sizeof(struct wsdd__ScopesType));
    memset(scope, 0, sizeof(struct wsdd__ScopesType));
    soap_default_wsdd__ScopesType(soap, scope);                                 // 设置寻找设备的范围
    scope->__item = (char*)soap_malloc(soap, strlen(SOAP_ITEM) + 1);
    memset(scope->__item, 0, strlen(SOAP_ITEM) + 1);
    strcpy(scope->__item, SOAP_ITEM);
    memset(&req, 0x00, sizeof(struct wsdd__ProbeType));
    soap_default_wsdd__ProbeType(soap, &req);
    req.Scopes = scope;
    req.Types  = (char*)soap_malloc(soap, strlen(SOAP_TYPES) + 1);     // 设置寻找设备的类型
    memset(req.Types, 0, strlen(SOAP_TYPES) + 1);
    strcpy(req.Types, SOAP_TYPES);
    int result = soap_send___wsdd__Probe(soap, SOAP_MCAST_ADDR, NULL, &req);     
    struct __wsdd__ProbeMatches rep; 
    struct wsdd__ProbeMatchType *probeMatch;
    unsigned int count = 0;
    int i;

    std::string strXAddr;
    while (SOAP_OK == result)                                                   // 开始循环接收设备发送过来的消息
    {
      memset(&rep, 0x00, sizeof(rep));
      result = soap_recv___wsdd__ProbeMatches(soap, &rep);
      if (SOAP_OK == result) {
          if (!soap->error) {
              string key = rep.wsdd__ProbeMatches->ProbeMatch->wsa__EndpointReference.Address;
              vector<string>::iterator iter=find(s_dev.begin(),s_dev.end(),key);
              if (iter!=s_dev.end())
                continue;
              else
                s_dev.push_back(key);
              if (NULL != rep.wsdd__ProbeMatches) {
                  count += rep.wsdd__ProbeMatches->__sizeProbeMatch;
                  for(i = 0; i < rep.wsdd__ProbeMatches->__sizeProbeMatch; i++) {
                      probeMatch = rep.wsdd__ProbeMatches->ProbeMatch + i;
                      strXAddr = probeMatch->XAddrs;
                      vecDevAddr.push_back(strXAddr);
                  }
              }
          }
      } else if (soap->error) {
          break;
      }
    }
    soap_destroy(soap);
    soap_end(soap);
}
