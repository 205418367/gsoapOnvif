#ifndef __ONVIFDEVICE_H
#define __ONVIFDEVICE_H
#pragma once
#include <iostream>
#include <algorithm>
#include <openssl/rsa.h>
#include <stdio.h>
#include <string>
#include <vector>

//控制方位
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define LEFTUP 4
#define LEFTDOWN 5
#define RIGHTUP 6
#define RIGHTDOWN 7
#define ZOOMIN 8
#define ZOOMOUT 9

//控制速度
#define LOWER 1
#define LOW 2
#define MEDIUM 3
#define FAST 4
#define FASTER 5

//预置点位
#define GOTO 0
#define SET 1
#define REMOVE 2
#define PRESET1 "1"
#define PRESET2 "2"
#define PRESET3 "3"
#define PRESET4 "4"
#define PRESET5 "5"
#define PRESET6 "6"
#define PRESET7 "7"
#define PRESET8 "8"
#define PRESET9 "9"
#define PRESET10 "10"

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
**command：UP DOWN LEFT RIGHT ZOOMIN ZOOMOUT
************************************************************************/
  int ptzContinuousMove(int command,int speed);

/************************************************************************
**函数：ptzContinuousStop
**功能：控制相机立即停止
************************************************************************/
  int ptzContinuousStop();

  int ptzRelativeMove(int command,int speed);
  int getProfile(string& profileToken);
  int getRTSPUrl(string& rtspUrl);
  int getIMAGEUrl(string& imageUrl);
/************************************************************************
**函数：ptzPreset
**功能：控制预置位
command: GOTO SET REMOVE
presettoken:PRESET1 PRESET2 ... PRESET10
************************************************************************/
  int ptzPreset(int command, string presettoken);
  int ptzPresetTour(vector<string> &PresetToken);
private:
  string m_deviceurl;
  string m_username;
  string m_passwd;
};
void detectDevice(vector<string>& vecDevAddr);
#endif
