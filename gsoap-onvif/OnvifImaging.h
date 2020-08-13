#ifndef __ONVIFIMAGE_H
#define __ONVIFIMAGE_H
#pragma once
#include <iostream>
#include <algorithm>
#include <openssl/rsa.h>
#include <stdio.h>
#include <string>
#include <vector>

//调焦
#define FOCUSIN 10
#define FOCUSOUT 11

typedef struct imagingsetting {
    int BacklightCompensationMode;
    float Brightness;
    float ColorSaturation;
    float Contrast;
    int ExposureMode;
    float ExposureTime;
    int AutoFocusMode;
    float Sharpness;
    int WhiteBalanceMode;
} ImagingSet;

class OnvifImaging{
public:
  OnvifImaging(std::string url,std::string username,std::string passwd);
  ~OnvifImaging();
  int getMediaUrl(std::string& mediaAddr);
  int GetVideoSources(std::string& videoSourceToken);
  int getIMAGEUrl(std::string& imageUrl);

  //功能：Continuous Focus Move command:FOCUSIN FOCUSOUT
  int ContinuousFocusMove(int command);
  //功能：Continuous Focus Stop
  int ContinuousFocusStop();
  // 1< speed <10
  int getFocusMoveSpeed(int& speed);
  int setFocusMoveSpeed(const int& speed);

  //功能：设置ImagingSettings
  int SetImagingSettings();
  //功能：获取ImagingSettings
  int GetImagingSettings(ImagingSet& imageSet);
  
private:
  std::string m_deviceurl;
  std::string m_username;
  std::string m_passwd;
  std::string mediaAddr;
  std::string imageUrl;
  std::string videoSourceToken;

  int focusMoveSpeed = 5;
  float Brightness = 50.0;
  float ColorSaturation = 35.0;
  float Contrast = 25.0;
  bool ForcePersistence = true; 
  float ExposureTime = 10000.0;
};
#endif
