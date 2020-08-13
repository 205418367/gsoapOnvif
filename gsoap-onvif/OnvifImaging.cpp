#include "OnvifImaging.h"
#include "wsseapi.h"
#include "soapMediaBindingProxy.h"
#include "soapDeviceBindingProxy.h"
#include "soapImagingBindingProxy.h"

OnvifImaging::OnvifImaging(std::string url,std::string username,std::string passwd):m_deviceurl(url),m_username(username),m_passwd(passwd){
    getMediaUrl(mediaAddr);
    getIMAGEUrl(imageUrl);
    GetVideoSources(videoSourceToken);
}
OnvifImaging::~OnvifImaging(){}


int OnvifImaging::setFocusMoveSpeed(const int& speed){
    focusMoveSpeed = speed;
    return 0;
}

int OnvifImaging::getFocusMoveSpeed(int& speed){
    speed = focusMoveSpeed;
    return 0;
}


int OnvifImaging::ContinuousFocusMove(int command){
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
         imaging.Focus->Continuous->Speed = -((float)focusMoveSpeed/10);
         break;
    case FOCUSOUT:
         imaging.Focus->Continuous->Speed = ((float)focusMoveSpeed/10);
         break;
    }
    int result = proxyImaging.Move(proxyImaging.soap_endpoint, NULL, &imaging, &response);
    if (SOAP_OK != result){
       return -1;
    }
    proxyImaging.destroy();
    return SOAP_OK;
}

int OnvifImaging::ContinuousFocusStop(){
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

int OnvifImaging::GetImagingSettings(ImagingSet& imageSet){
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
    imageSet.BacklightCompensationMode = Response.ImagingSettings->BacklightCompensation->Mode;
    imageSet.ExposureMode = Response.ImagingSettings->Exposure->Mode;
    imageSet.AutoFocusMode = Response.ImagingSettings->Focus->AutoFocusMode;
    imageSet.WhiteBalanceMode = Response.ImagingSettings->WhiteBalance->Mode;

    float* Brightness = Response.ImagingSettings->Brightness;
    imageSet.Brightness = *Brightness;
    float* ColorSaturation = Response.ImagingSettings->ColorSaturation;
    imageSet.ColorSaturation = *ColorSaturation;
    float* Contrast = Response.ImagingSettings->Contrast;
    imageSet.Contrast = *Contrast;
    float* ExposureTime = Response.ImagingSettings->Exposure->ExposureTime;
    imageSet.ExposureTime = *ExposureTime;
    float* Sharpness = Response.ImagingSettings->Sharpness;
    imageSet.Sharpness = *Sharpness;
    proxyImaging.destroy();
    return SOAP_OK;
}

int OnvifImaging::SetImagingSettings(){
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
    //imaging.ImagingSettings->Brightness = &Brightness;
    //imaging.ImagingSettings->ColorSaturation = &ColorSaturation;
    //imaging.ImagingSettings->Contrast = &Contrast;
    imaging.ImagingSettings->Exposure = soap_new_tt__Exposure20(proxyImaging.soap, -1);
    imaging.ImagingSettings->Exposure->Mode = tt__ExposureMode__MANUAL;
    //imaging.ImagingSettings->Exposure->ExposureTime = &ExposureTime;
    //imaging.ForcePersistence = &ForcePersistence;

    imaging.ImagingSettings->Focus = soap_new_tt__FocusConfiguration20(proxyImaging.soap, -1);
    imaging.ImagingSettings->Focus->AutoFocusMode = tt__AutoFocusMode__MANUAL;
    int result = proxyImaging.SetImagingSettings(proxyImaging.soap_endpoint,NULL,&imaging, &response);
    if (SOAP_OK != result){
       return -1;
    }
    proxyImaging.destroy();
    return SOAP_OK;
}


int OnvifImaging::getIMAGEUrl(std::string& imageUrl){
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


int OnvifImaging::GetVideoSources(std::string& videoSourceToken){
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


int OnvifImaging::getMediaUrl(std::string& mediaAddr){
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
