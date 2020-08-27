#include "OnvifPTZ.h"
#include "wsseapi.h"
#include "soapDeviceBindingProxy.h"
#include "soapMediaBindingProxy.h"
#include "soapPTZBindingProxy.h"

OnvifPTZ::OnvifPTZ(std::string url,std::string username,std::string passwd):m_deviceurl(url),m_username(username),m_passwd(passwd){
    getMediaUrl(mediaAddr);
    getPTZUrl(PTZAddr);
    getProfile(strProfileToken);
}
OnvifPTZ::~OnvifPTZ(){}


int OnvifPTZ::setPtzMoveSpeed(const int& speed){
    ptzMoveSpeed = speed;
    return 0;
}

int OnvifPTZ::getPtzMoveSpeed(int& speed){
    speed = ptzMoveSpeed;
    return 0;
}


int OnvifDevice::GetOnePresets(const string& pretoken,string& prename,float& p,float& t,float& z){  
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
           prename = (*name).erase(0, 2);
	   p = PanTilt->x;
	   t = PanTilt->y;
           z = Zoom->x;
	}
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifPTZ::GetAllPresets(std::vector<PreInfo>& allpresets){
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
	std::string *name = response.Preset[i]->Name;
	std::string *token = response.Preset[i]->token;
        
	if ((*name).find("PR") != std::string::npos){
           tt__Vector2D* panTilt = response.Preset[i]->PTZPosition->PanTilt;
	   tt__Vector1D* zoom = response.Preset[i]->PTZPosition->Zoom;
           std::cout<<"name: "<<*name<<std::endl;
        std::cout<<"token: "<<*token<<std::endl;
           float x = panTilt->x;
           std::cout<<"x: "<<x<<std::endl;
	   float y = panTilt->y;
           std::cout<<"y: "<<x<<std::endl;
           float z = zoom->x;
           std::cout<<"z: "<<x<<std::endl;

           PreInfo preinfo;
	   preinfo.token = *token;
           preinfo.name = *name.erase(0, 2);
           preinfo.p = x;
           preinfo.t = y;
           preinfo.z = z;
           allpresets.push_back(preinfo);
	}
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifPTZ::SetPreset(std::string presetToken,std::string presetName){
    PTZBindingProxy proxyPTZ;
    proxyPTZ.soap_endpoint = PTZAddr.c_str();
    soap_register_plugin(proxyPTZ.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyPTZ.soap, NULL, m_username.c_str(), m_passwd.c_str())){
         return -1;
    }
    _tptz__SetPreset preset;
    _tptz__SetPresetResponse response;
    preset.ProfileToken = strProfileToken;
    std::string Name = "PR"+presetName;
    preset.PresetName = &Name;
    preset.PresetToken = &presetToken;
    int result = proxyPTZ.SetPreset(proxyPTZ.soap_endpoint,NULL,&preset, &response);
    if (SOAP_OK != result){
         return -1;
    }
    proxyPTZ.destroy();
    return SOAP_OK;
}


int OnvifPTZ::ptzPreset(int command, std::string presetToken){
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
        preset.Speed = soap_new_tt__PTZSpeed(proxyPTZ.soap, -1);
        preset.Speed->PanTilt = soap_new_tt__Vector2D(proxyPTZ.soap, -1);
        std::string str="http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace";
        preset.Speed->PanTilt->space =&str;
        preset.Speed->Zoom = soap_new_tt__Vector1D(proxyPTZ.soap, -1);
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


int OnvifPTZ::GotoHomePosition(){
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


int OnvifPTZ::ptzRelativeMove(int command){
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


int OnvifPTZ::ptzContinuousMove(int command){
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


int OnvifPTZ::ptzContinuousStop(){ 
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


int OnvifPTZ::getMediaUrl(std::string& mediaAddr){
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


int OnvifPTZ::getProfile(std::string& profileToken){
    MediaBindingProxy proxyMedia;
    proxyMedia.soap_endpoint = mediaAddr.c_str();
    soap_register_plugin(proxyMedia.soap, soap_wsse);
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, m_username.c_str(), m_passwd.c_str())){
		return -1;
    }
    _trt__GetProfiles           Profiles_req;
    _trt__GetProfilesResponse   Profiles_rep;
    int result = proxyMedia.GetProfiles(proxyMedia.soap_endpoint, NULL, &Profiles_req, &Profiles_rep);
    if (SOAP_OK != result){	
          return -1;
    }
    profileToken = Profiles_rep.Profiles[0]->token;
    proxyMedia.destroy();
    return SOAP_OK;
}


int OnvifPTZ::getPTZUrl(std::string& PTZAddr){
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
