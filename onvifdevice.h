#ifndef __ONVIFDEVICE_H
#define __ONVIFDEVICE_H
#pragma once
#include <iostream>
#include <algorithm>
#include <openssl/rsa.h>
#include <stdio.h>
#include <string>
#include <vector>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define ZOOMIN 4
#define ZOOMOUT 5
#define speed 1
#define GOTO 0
#define SET 1
#define REMOVE 2

#define MAX_HOSTNAME_LEN 128
#define MAX_LOGMSG_LEN 256 

#define SOAP_TO         "urn:schemas-xmlsoap-org:ws:2005:04:discovery"
#define SOAP_ACTION     "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"
#define SOAP_MCAST_ADDR "soap.udp://239.255.255.250:3702"                       // onvif规定的组播地址
#define SOAP_ITEM       ""                                                      // 寻找的设备范围
#define SOAP_TYPES      "dn:NetworkVideoTransmitter"                            // 寻找的设备类型
#define GET 3
using namespace std;
class OnvifDevice{
public:
  OnvifDevice(string url,string username,string passwd);
  ~OnvifDevice();
  int getMediaUrl(string& mediaAddr);
  int getPTZUrl(string& PTZAddr);
  int getDateTime(int& year, int& month, int& day,
                  int& hour, int& minute, int& second);
  int setDateTime();
  int getDeviceInformation(string& Manufacturer,string& Model,
           string& FirmwareVersion,string& SerialNumber,string& HardwareId);

/************************************************************************
**函数：ptzContinuousMove
**功能：控制相机持续转动
**command：UP DOWN LEFT RIGHT ZOOMIN ZOOMOUT speed 
************************************************************************/
  int ptzContinuousMove(int command);

/************************************************************************
**函数：ptzContinuousStop
**功能：控制相机立即停止
************************************************************************/
  int ptzContinuousStop();

  int ptzRelativeMove(int command);
  int getProfile(string& profileToken);
  int getRTSPUrl(string& rtspUrl);
  int getIMAGEUrl(string& imageUrl);
  int ptzPreset(int command, string presettoken);
  int ptzPresetTour(vector<string> &PresetToken);
private:
  string m_deviceurl;
  string m_username;
  string m_passwd;
};
void detectDevice(vector<string>& vecDevAddr);
#endif
