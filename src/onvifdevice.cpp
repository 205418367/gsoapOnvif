#include "onvifdevice.h"
#include "stdio.h"
#include "wsdd.nsmap"
#include "wsseapi.h"
#include "wsaapi.h"
#include "ErrorLog.h"
#include "soapDeviceBindingProxy.h"
#include "soapMediaBindingProxy.h"
#include "soapPTZBindingProxy.h"
#include "soapImagingBindingProxy.h"
#include "soapPullPointSubscriptionBindingProxy.h"
#include "soapRemoteDiscoveryBindingProxy.h"
#include "gsoap-onvif/onvif/soapStub.h"

OnvifDevice::OnvifDevice(string url,string username,string passwd):m_deviceurl(url),m_username(username),m_passwd(passwd){
    getMediaUrl(mediaAddr);
    getPTZUrl(PTZAddr);
    getIMAGEUrl(imageUrl);
    getProfile(strProfileToken);
    GetVideoSources(videoSourceToken);
}
OnvifDevice::~OnvifDevice(){}


int OnvifDevice::setPtzMoveSpeed(const int& speed){
    ptzMoveSpeed = speed;
    return 0;
}

int OnvifDevice::getPtzMoveSpeed(int& speed){
    speed = ptzMoveSpeed;
    return 0;
}

int OnvifDevice::setFocusMoveSpeed(const int& speed){
    focusMoveSpeed = speed;
    return 0;
}

int OnvifDevice::getFocusMoveSpeed(int& speed){
    speed = focusMoveSpeed;
    return 0;
}


int OnvifDevice::ContinuousFocusMove(int command){
    ImagingBindingProxy proxyImaging;
    proxyImaging.soap_endpoint = imageUrl.c_str();
    soap_register_plugin(proxyImaging.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _timg__Move           imaging;
    _timg__MoveResponse   response;
    imaging.VideoSourceToken = videoSourceToken;
    imaging.Focus = soap_new_tt__FocusMove(proxyImaging.soap, -1);
    imaging.Focus->Continuous = soap_new_tt__ContinuousFocus(proxyImaging.soap, -1);
    switch (command){
        case FOCUSIN:
            imaging.Focus->Continuous->Speed = -((float)focusMoveSpeed/7);
            break;
        case FOCUSOUT:
            imaging.Focus->Continuous->Speed = ((float)focusMoveSpeed/7);
            break;
    }
    int result = proxyImaging.Move(proxyImaging.soap_endpoint, NULL, &imaging, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyImaging.destroy();
    return SOAP_OK;
}

int OnvifDevice::ContinuousFocusStop(){
    ImagingBindingProxy proxyImaging;
    proxyImaging.soap_endpoint = imageUrl.c_str();
    soap_register_plugin(proxyImaging.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _timg__Stop           imaging;
    _timg__StopResponse   response;
    imaging.VideoSourceToken = videoSourceToken;
    int result = proxyImaging.Stop(proxyImaging.soap_endpoint, NULL, &imaging, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyImaging.destroy();
    return SOAP_OK;
}

int OnvifDevice::GetImagingSettings(int command, ImagingSet& imageSet){
    ImagingBindingProxy proxyImaging;
    proxyImaging.soap_endpoint = imageUrl.c_str();
    soap_register_plugin(proxyImaging.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _timg__GetImagingSettings Imaging;
    _timg__GetImagingSettingsResponse Response;
    Imaging.VideoSourceToken = videoSourceToken;
    int result = proxyImaging.GetImagingSettings(proxyImaging.soap_endpoint,NULL,&Imaging, &Response);
    if (SOAP_OK != result){
        return -1;
    }
    //imageSet.WhiteBalanceMode = Response.ImagingSettings->WhiteBalance->Mode;
    //imageSet.BacklightCompensationMode = Response.ImagingSettings->BacklightCompensation->Mode;

    switch (command){
        case exposureMode:
            imageSet.ExposureMode = Response.ImagingSettings->Exposure->Mode;
            break;
        case autoFocusMode:
            imageSet.AutoFocusMode = Response.ImagingSettings->Focus->AutoFocusMode;
            break;
        case exposureTime:
            imageSet.ExposureMode = Response.ImagingSettings->Exposure->Mode;
            float* ExposureTime = Response.ImagingSettings->Exposure->ExposureTime;
            imageSet.ExposureTime = *ExposureTime;
            break;
    }
    //float* Brightness = Response.ImagingSettings->Brightness;
    //imageSet.Brightness = *Brightness;
    //float* ColorSaturation = Response.ImagingSettings->ColorSaturation;
    //imageSet.ColorSaturation = *ColorSaturation;
    //float* Contrast = Response.ImagingSettings->Contrast;
    //imageSet.Contrast = *Contrast;
    //float* Sharpness = Response.ImagingSettings->Sharpness;
    //imageSet.Sharpness = *Sharpness;
    proxyImaging.destroy();
    return SOAP_OK;
}

int OnvifDevice::SetImagingSettings(int command, ImagingSet& imageSet){
    ImagingBindingProxy proxyImaging;
    proxyImaging.soap_endpoint = imageUrl.c_str();
    soap_register_plugin(proxyImaging.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _timg__SetImagingSettings           imaging;
    _timg__SetImagingSettingsResponse   response;
    imaging.VideoSourceToken = videoSourceToken;
    imaging.ImagingSettings = soap_new_tt__ImagingSettings20(proxyImaging.soap, -1);

    float ExposureTime = imageSet.ExposureTime;
    //imaging.ImagingSettings->Brightness = &Brightness;
    //imaging.ImagingSettings->ColorSaturation = &ColorSaturation;
    //imaging.ImagingSettings->Contrast = &Contrast;
    //imaging.ForcePersistence = &ForcePersistence;

    switch (command){
        case exposureAuto:
            imaging.ImagingSettings->Exposure = soap_new_tt__Exposure20(proxyImaging.soap, -1);
            imaging.ImagingSettings->Exposure->Mode = tt__ExposureMode__AUTO;
            break;
        case exposureTime:
            imaging.ImagingSettings->Exposure = soap_new_tt__Exposure20(proxyImaging.soap, -1);
            imaging.ImagingSettings->Exposure->Mode = tt__ExposureMode__MANUAL;
            imaging.ImagingSettings->Exposure->ExposureTime = &ExposureTime;
            //imaging.ImagingSettings->Exposure->Iris = &Iris;
            break;
        case focusAuto:
            imaging.ImagingSettings->Focus = soap_new_tt__FocusConfiguration20(proxyImaging.soap, -1);
            imaging.ImagingSettings->Focus->AutoFocusMode = tt__AutoFocusMode__AUTO;
            break;
        case focusManual:
            imaging.ImagingSettings->Focus = soap_new_tt__FocusConfiguration20(proxyImaging.soap, -1);
            imaging.ImagingSettings->Focus->AutoFocusMode = tt__AutoFocusMode__MANUAL;
            break;
    }
    int result = proxyImaging.SetImagingSettings(proxyImaging.soap_endpoint,NULL,&imaging, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyImaging.destroy();
    return SOAP_OK;
}

// c++ return 只能返回一个值；数值类型只用于右值就不需要引用
PreInfo calculatePTZ(PreInfo& info,float x,float y,float z){
    float p,t,zo;
    // x
    if(x==0.0){
        p = 0.0;
    }else if(x<=1800.0){
        p = x/1800.0;
    }else{
        p = (x-1800.0)/1800.0-1.0;
    }
    // y
    if(y==0.0){
        t = 0.0;
    }else if(y>1080.0){
        t = 0.5-(y-2700.0)/1800.0;
    }else{
        t = -y/1800.0;
    }
    // z
    zo = (z-10.0)/290.0;
    info.p = p;
    info.t = t;
    info.z = zo;
    return info;
}

int OnvifDevice::GetOnePresets(const string& pretoken,PreInfo& preinfo){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__GetPresets preset;
    _tptz__GetPresetsResponse response;
    preset.ProfileToken = strProfileToken;
    int result = proxyPTZ.GetPresets(proxyPTZ.soap_endpoint,NULL,&preset, &response);
    if (SOAP_OK != result){
        return -1;
    }
    for (int i = 0;i<response.Preset.size();i++){
        string *name = response.Preset[i]->Name;
        string *token = response.Preset[i]->token;
        if (((*token)==pretoken)){
            tt__Vector2D* PanTilt = response.Preset[i]->PTZPosition->PanTilt;
            tt__Vector1D* Zoom = response.Preset[i]->PTZPosition->Zoom;
            PreInfo info;
            info.name = (*name).erase(0, 2);
            float x = PanTilt->x;
            float y = PanTilt->y;
            float z = Zoom->x;
            preinfo = calculatePTZ(info,x,y,z);
        }
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::GetAllPresets(vector<PreInfo>& allpresets){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__GetPresets preset;
    _tptz__GetPresetsResponse response;
    preset.ProfileToken = strProfileToken;
    int result = proxyPTZ.GetPresets(proxyPTZ.soap_endpoint,NULL,&preset, &response);
    if (SOAP_OK != result){
        return -1;
    }
    for (int i = 0;i<response.Preset.size();i++){
        string *name = response.Preset[i]->Name;
        string *token = response.Preset[i]->token;
        if (!(*name).find("PR")){
            tt__Vector2D* PanTilt = response.Preset[i]->PTZPosition->PanTilt;
            tt__Vector1D* Zoom = response.Preset[i]->PTZPosition->Zoom;
            PreInfo preinfo;
            preinfo.token = *token;
            preinfo.name = (*name).erase(0, 2);
            float x = PanTilt->x;
            float y = PanTilt->y;
            float z = Zoom->x;
            PreInfo Info = calculatePTZ(preinfo,x,y,z);
            allpresets.push_back(Info);
        }
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::SetPreset(string presetToken,string presetName){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__SetPreset preset;
    _tptz__SetPresetResponse response;
    preset.ProfileToken = strProfileToken;
    string Name = "PR"+presetName;
    preset.PresetName = &Name;
    preset.PresetToken = &presetToken;
    int result = proxyPTZ.SetPreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}

int OnvifDevice::ptzPreset(int command, string presetToken){
    int result = 0;
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
        //preset.Speed = soap_new_tt__PTZSpeed(proxyPTZ.soap, -1);
        //preset.Speed->PanTilt = soap_new_tt__Vector2D(proxyPTZ.soap, -1);
        //std::string str="http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace";
        //preset.Speed->PanTilt->space =&str;
        //preset.Speed->Zoom = soap_new_tt__Vector1D(proxyPTZ.soap, -1);
        result = proxyPTZ.GotoPreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
        if (SOAP_OK != result){
            return -1;
        }
    }else if (command == REMOVE){
        _tptz__RemovePreset preset;
        _tptz__RemovePresetResponse response;
        preset.ProfileToken = strProfileToken;
        preset.PresetToken = presetToken;
        result = proxyPTZ.RemovePreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
        if (SOAP_OK != result){
            return -1;
        }
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::GotoHomePosition(){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__GotoHomePosition HomePosition;
    _tptz__GotoHomePositionResponse Response;
    HomePosition.ProfileToken = strProfileToken;
    int result = proxyPTZ.GotoHomePosition(proxyPTZ.soap_endpoint,NULL,&HomePosition, &Response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::SetHomePosition(){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__SetHomePosition HomePosition;
    _tptz__SetHomePositionResponse Response;
    HomePosition.ProfileToken = strProfileToken;
    int result = proxyPTZ.SetHomePosition(proxyPTZ.soap_endpoint,NULL,&HomePosition, &Response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}

int OnvifDevice::ptzRelativeMove(int command){
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
            ptz_req.Translation->PanTilt->x = -((float)ptzMoveSpeed / 7);
            ptz_req.Translation->PanTilt->y = 0;
            break;
        case RIGHT:
            ptz_req.Translation->PanTilt->x = ((float)ptzMoveSpeed / 7);
            ptz_req.Translation->PanTilt->y = 0;
            break;
        case UP:
            ptz_req.Translation->PanTilt->x = 0;
            ptz_req.Translation->PanTilt->y = ((float)ptzMoveSpeed / 7);
            break;
        case DOWN:
            ptz_req.Translation->PanTilt->x = 0;
            ptz_req.Translation->PanTilt->y = -((float)ptzMoveSpeed / 7);
            break;
        case LEFTUP:
            ptz_req.Translation->PanTilt->x = -((float)ptzMoveSpeed / 7);
            ptz_req.Translation->PanTilt->y = ((float)ptzMoveSpeed / 7);
            break;
        case LEFTDOWN:
            ptz_req.Translation->PanTilt->x = -((float)ptzMoveSpeed / 7);
            ptz_req.Translation->PanTilt->y = -((float)ptzMoveSpeed / 7);
            break;
        case RIGHTUP:
            ptz_req.Translation->PanTilt->x = ((float)ptzMoveSpeed / 7);
            ptz_req.Translation->PanTilt->y = ((float)ptzMoveSpeed / 7);
            break;
        case RIGHTDOWN:
            ptz_req.Translation->PanTilt->x = ((float)ptzMoveSpeed / 7);
            ptz_req.Translation->PanTilt->y = -((float)ptzMoveSpeed / 7);
            break;
        case ZOOMIN:
            ptz_req.Translation->Zoom->x = ((float)ptzMoveSpeed / 7);
            break;
        case ZOOMOUT:
            ptz_req.Translation->Zoom->x = -((float)ptzMoveSpeed / 7);
            break;
        default:
            break;
    }
    int result = proxyPTZ.RelativeMove(proxyPTZ.soap_endpoint,NULL,&ptz_req, &ptz_rep);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::ptzContinuousMove(int command){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL,  m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__ContinuousMove continuousMove;
    _tptz__ContinuousMoveResponse response;
    continuousMove.ProfileToken = strProfileToken;
    continuousMove.Velocity = soap_new_tt__PTZSpeed(proxyPTZ.soap, -1);
    continuousMove.Velocity->PanTilt = soap_new_tt__Vector2D(proxyPTZ.soap, -1);
    std::string str="http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace";
    continuousMove.Velocity->PanTilt->space =&str;
    continuousMove.Velocity->Zoom = soap_new_tt__Vector1D(proxyPTZ.soap, -1);

    switch (command){
        case LEFT:
            continuousMove.Velocity->PanTilt->x = -((float)ptzMoveSpeed / 7);
            continuousMove.Velocity->PanTilt->y = 0;
            break;
        case RIGHT:
            continuousMove.Velocity->PanTilt->x = ((float)ptzMoveSpeed / 7);
            continuousMove.Velocity->PanTilt->y = 0;
            break;
        case UP:
            continuousMove.Velocity->PanTilt->x = 0;
            continuousMove.Velocity->PanTilt->y = ((float)ptzMoveSpeed / 7);
            break;
        case DOWN:
            continuousMove.Velocity->PanTilt->x = 0;
            continuousMove.Velocity->PanTilt->y = -((float)ptzMoveSpeed / 7);
            break;
        case LEFTUP:
            continuousMove.Velocity->PanTilt->x = -((float)ptzMoveSpeed / 7);
            continuousMove.Velocity->PanTilt->y = ((float)ptzMoveSpeed / 7);
            break;
        case LEFTDOWN:
            continuousMove.Velocity->PanTilt->x = -((float)ptzMoveSpeed / 7);
            continuousMove.Velocity->PanTilt->y = -((float)ptzMoveSpeed / 7);
            break;
        case RIGHTUP:
            continuousMove.Velocity->PanTilt->x = ((float)ptzMoveSpeed / 7);
            continuousMove.Velocity->PanTilt->y = ((float)ptzMoveSpeed / 7);
            break;
        case RIGHTDOWN:
            continuousMove.Velocity->PanTilt->x = ((float)ptzMoveSpeed / 7);
            continuousMove.Velocity->PanTilt->y = -((float)ptzMoveSpeed / 7);
            break;
        case ZOOMIN:
            continuousMove.Velocity->Zoom->x = ((float)ptzMoveSpeed / 7);
            break;
        case ZOOMOUT:
            continuousMove.Velocity->Zoom->x = -((float)ptzMoveSpeed / 7);
            break;
        default:
            break;
    }
    int result = proxyPTZ.ContinuousMove(proxyPTZ.soap_endpoint,NULL,&continuousMove, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::ptzAbsoluteMove(float& p, float& t, float& z){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL,  m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__AbsoluteMove absoluteMove;
    _tptz__AbsoluteMoveResponse response;
    absoluteMove.ProfileToken = strProfileToken;
    absoluteMove.Position = soap_new_tt__PTZVector(proxyPTZ.soap, -1);
    absoluteMove.Position->PanTilt = soap_new_tt__Vector2D(proxyPTZ.soap, -1);
    absoluteMove.Position->PanTilt->x = p;
    absoluteMove.Position->PanTilt->y = t;
    absoluteMove.Position->Zoom = soap_new_tt__Vector1D(proxyPTZ.soap, -1);
    absoluteMove.Position->Zoom->x = z;
    int result = proxyPTZ.AbsoluteMove(proxyPTZ.soap_endpoint,NULL,&absoluteMove, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}

int OnvifDevice::ptzContinuousStop(){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL,  m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tptz__Stop stopImmediate;
    _tptz__StopResponse response;
    stopImmediate.ProfileToken = strProfileToken;
    int result = proxyPTZ.Stop(proxyPTZ.soap_endpoint,NULL,&stopImmediate, &response);
    if (SOAP_OK != result){
        return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifDevice::getRTSPUrl(string& rtspUrl){
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
    StreamUri_req.ProfileToken = strProfileToken;
    int result = proxyMedia.GetStreamUri(proxyMedia.soap_endpoint, NULL, &StreamUri_req, &StreamUri_rep);
    if (SOAP_OK != result){
        return -1;
    }
    rtspUrl = StreamUri_rep.MediaUri->Uri;
    proxyMedia.destroy();
    return SOAP_OK;
}


int OnvifDevice::getIMAGEUrl(string& imageUrl){
    DeviceBindingProxy proxyDevice;
    proxyDevice.soap_endpoint = m_deviceurl.c_str();
    soap_register_plugin(proxyDevice.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _tds__GetCapabilities           IMAGEUrl;
    _tds__GetCapabilitiesResponse   response;
    int result = proxyDevice.GetCapabilities(proxyDevice.soap_endpoint, NULL, &IMAGEUrl, &response);
    if (SOAP_OK != result){
        return -1;
    }
    imageUrl = response.Capabilities->Imaging->XAddr;
    proxyDevice.destroy();
    return SOAP_OK;
}


int OnvifDevice::getProfile(string& profileToken){
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _trt__GetProfiles           Profiles_req;
    //存储的是获取回来的信息
    _trt__GetProfilesResponse   Profiles_rep;
    int result = proxyMedia.GetProfiles(proxyMedia.soap_endpoint, NULL, &Profiles_req, &Profiles_rep);
    if (SOAP_OK != result){
        return -1;
    }
    // 可能会有好几路流，相应的也会有好几个profile,这里只取第一路码流
    profileToken = Profiles_rep.Profiles[0]->token;
    proxyMedia.destroy();
    return SOAP_OK;
}


int OnvifDevice::GetVideoSources(string& videoSourceToken){
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _trt__GetVideoSources VideoSources;
    _trt__GetVideoSourcesResponse response;
    int result = proxyMedia.GetVideoSources(proxyMedia.soap_endpoint, NULL, &VideoSources, &response);
    if (SOAP_OK != result){
        return -1;
    }
    videoSourceToken = response.VideoSources[0]->token;
    proxyMedia.destroy();
    return SOAP_OK;
}


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
    proxyDevice.destroy();
    return SOAP_OK;
}


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
    proxyDevice.destroy();
    return SOAP_OK;
}


int OnvifDevice::GetSnapshotUri(string& shotUri){
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
        return -1;
    }
    _trt__GetSnapshotUri Snapshot;
    _trt__GetSnapshotUriResponse response;
    int result = proxyMedia.GetSnapshotUri(proxyMedia.soap_endpoint, NULL, &Snapshot, &response);
    if (SOAP_OK != result){
        return -1;
    }
    shotUri = response.MediaUri->Uri;
    proxyMedia.destroy();
    return SOAP_OK;
}

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
