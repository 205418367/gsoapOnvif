#ifndef __OnvifPTZ_H
#define __OnvifPTZ_H
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

//预置点位
#define GOTO 0
#define REMOVE 1

typedef struct presetsinfo {
    std::string token;
    std::string name;
    float p;
    float t;
    float z;
} PreInfo;


class OnvifPTZ{
public:
  OnvifPTZ(std::string url,std::string username,std::string passwd);
  ~OnvifPTZ();
  int getMediaUrl(std::string& mediaAddr);
  int getProfile(std::string& profileToken);
  int getPTZUrl(std::string& PTZAddr);

  //功能：控制相机持续转动; 停止相机需要调用ptzContinuousStop接口
  // speed:LOWER LOW MEDIUM FAST FASTER
  // command:UP DOWN LEFT RIGHT LEFTUP LEFTDOWN RIGHTUP RIGHTDOWN ZOOMIN ZOOMOUT
  int ptzContinuousMove(int command);
  int ptzContinuousStop();
  //功能：控制相机转动一定角度,转动完毕会自动停止
  // command:UP DOWN LEFT RIGHT LEFTUP LEFTDOWN RIGHTUP RIGHTDOWN ZOOMIN ZOOMOUT
  int ptzRelativeMove(int command);
  //功能：复位
  int GotoHomePosition();
  // 1< speed <7
  int getPtzMoveSpeed(int& speed);
  int setPtzMoveSpeed(const int& speed);
  

  //功能：控制预置位; command: GOTO REMOVE presettoken:1~300
  int ptzPreset(int command, std::string presettoken);
  //功能：根据预置点token查询name和position
  int GetOnePresets(const std::string& pretoken,float& x,float& y,float& z,std::string& prename);
  //功能：查询所有已设置的预置点token、name和ptz值
  int GetAllPresets(std::vector<PreInfo>& allpresets);
  //功能：设置预置点位。presetToken:1~300 ; presetName:name
  int SetPreset(std::string presetToken,std::string presetName);
  
private:
  std::string m_deviceurl;
  std::string m_username;
  std::string m_passwd;
  std::string mediaAddr;
  std::string PTZAddr;
  std::string strProfileToken;
  int ptzMoveSpeed = 3;
};
#endif
