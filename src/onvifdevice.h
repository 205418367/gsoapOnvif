#ifndef __ONVIFDEVICE_H
#define __ONVIFDEVICE_H
#pragma once
#include <iostream>
#include <algorithm>
#include <openssl/rsa.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "soapPrint.h"

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
#define FOCUSIN 10
#define FOCUSOUT 11

//预置点位
#define GOTO 12
#define REMOVE 13

//ImagingSettings
#define exposureAuto 14
#define exposureTime 15
#define focusAuto 16
#define focusManual 17
#define exposureMode 18
#define autoFocusMode 19

#define MAX_HOSTNAME_LEN 128
#define MAX_LOGMSG_LEN 256

#define SOAP_TO         "urn:schemas-xmlsoap-org:ws:2005:04:discovery"
#define SOAP_ACTION     "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"
#define SOAP_MCAST_ADDR "soap.udp://239.255.255.250:3702"                       // onvif规定的组播地址
#define SOAP_ITEM       ""                                                      // 寻找的设备范围
#define SOAP_TYPES      "dn:NetworkVideoTransmitter"                            // 寻找的设备类型
#define GET 3

typedef struct presetsinfo {
    std::string token;
    std::string name;
    float p;
    float t;
    float z;
} PreInfo;

typedef struct imagingsetting {
    int BacklightCompensationMode;
    float Brightness;
    float ColorSaturation;
    float Contrast;
    int ExposureMode;   // 0: auto 1:manual
    float ExposureTime; // 5000 <ExposureTime <= 15000
    int AutoFocusMode;  // 0: auto 1:manual
    float Sharpness;
    int WhiteBalanceMode;
} ImagingSet;

using namespace std;
class OnvifDevice{
public:
    OnvifDevice(string url,string username,string passwd);
    ~OnvifDevice();

    int getMediaUrl(string& mediaAddr);
    int getProfile(string& profileToken);
    int getPTZUrl(string& PTZAddr);
    int GetVideoSources(string& videoSourceToken);
    int GetSnapshotUri(string& shotUri);
    int getRTSPUrl(string& rtspUrl);
    int getIMAGEUrl(string& imageUrl);

    //功能：控制相机持续转动; 停止相机需要调用ptzContinuousStop接口
    // speed:LOWER LOW MEDIUM FAST FASTER
    // command:UP DOWN LEFT RIGHT LEFTUP LEFTDOWN RIGHTUP RIGHTDOWN ZOOMIN ZOOMOUT
    int ptzContinuousMove(int command);
    int ptzContinuousStop();
    //功能：绝对运动 0<= p、t、z <= 1
    int ptzAbsoluteMove(float& p, float& t, float& z);
    // 1< speed <7
    int getPtzMoveSpeed(int& speed);
    int setPtzMoveSpeed(const int& speed);
    //功能：控制相机转动一定角度,转动完毕会自动停止
    // command:UP DOWN LEFT RIGHT LEFTUP LEFTDOWN RIGHTUP RIGHTDOWN ZOOMIN ZOOMOUT
    int ptzRelativeMove(int command);

    //功能：控制预置位; command: GOTO REMOVE presettoken:1~300
    int ptzPreset(int command, string presettoken);
    //功能：根据预置点token查询name和position
    int GetOnePresets(const string& pretoken,PreInfo& preinfo);
    //功能：查询所有已设置的预置点token、name和ptz值
    int GetAllPresets(vector<PreInfo>& allpresets);
    //功能：设置预置点位。presetToken:1~300 ; presetName:name
    int SetPreset(string presetToken,string presetName);
    //功能：Continuous Focus Move command:FOCUSIN FOCUSOUT
    int ContinuousFocusMove(int command);
    //功能：Continuous Focus Stop
    int ContinuousFocusStop();
    // 1< speed <7
    int getFocusMoveSpeed(int& speed);
    int setFocusMoveSpeed(const int& speed);

    //功能：goto Home位置
    int GotoHomePosition();
    //功能：设置 (0,0,0)为Home位置
    int SetHomePosition();
    //command: exposureAuto exposureTime focusAuto focusManual;
    int SetImagingSettings(int command, ImagingSet& imageSet);
    //command: exposureMode autoFocusMode  exposureTime
    int GetImagingSettings(int command, ImagingSet& imageSet);

private:
    string m_deviceurl;
    string m_username;
    string m_passwd;
    string mediaAddr;
    string PTZAddr;
    string imageUrl;
    string strProfileToken;
    string videoSourceToken;

    int ptzMoveSpeed = 3;
    int focusMoveSpeed = 3;
};
//功能: 发现设备获取设备地址
void detectDevice(vector<string>& vecDevAddr);
#endif
