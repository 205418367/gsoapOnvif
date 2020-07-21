cmake_minimum_required(VERSION 3.10)
project(ONVIF_CLEINT)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -o0 -g -ggdb -Wall -w -fPIC -fno-use-cxa-atexit -fexceptions")
add_definitions(-DWITH_NONAMESPACES -DWITH_DOM -DWITH_OPENSSL -DWITH_COOKIES)

include_directories(gsoap-onvif/onvif
                    gsoap-onvif/openssl
                    gsoap-onvif/plugin
                    gsoap-onvif/source/include
                    ${PROJECT_SOURCE_DIR}/
                    )
set(ProxyOBJ 
    gsoap-onvif/source/src/soapDeviceBindingProxy.cpp
    gsoap-onvif/source/src/soapMediaBindingProxy.cpp
    gsoap-onvif/source/src/soapPTZBindingProxy.cpp
    gsoap-onvif/source/src/soapPullPointSubscriptionBindingProxy.cpp
    gsoap-onvif/source/src/soapRemoteDiscoveryBindingProxy.cpp
    )
set(PluginOBJ
    gsoap-onvif/plugin/wsaapi.cpp
    gsoap-onvif/plugin/wsseapi.cpp
    gsoap-onvif/plugin/threads.cpp
    gsoap-onvif/plugin/duration.cpp
    gsoap-onvif/plugin/smdevp.cpp
    gsoap-onvif/plugin/mecevp.cpp
    gsoap-onvif/plugin/dom.cpp
    gsoap-onvif/plugin/stdsoap2.cpp
    )
set(Onvifobj
    gsoap-onvif/onvif/soapC.cpp
    gsoap-onvif/onvif/soapClient.cpp
    gsoap-onvif/onvif/wsdd.nsmap
    )

#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdlib/armeabi-v7a) 
#add_executable(client proxy/main.cpp proxy/onvifdevice.cpp 
#	        proxy/onvifcomm.cpp ${ProxyOBJ} ${PluginOBJ} ${Onvifobj})
add_library(onvifdevice SHARED onvifdevice.cpp ${ProxyOBJ} ${PluginOBJ} ${Onvifobj})
target_link_libraries(onvifdevice ssl)
add_library(onvifcomm SHARED onvifcomm.cpp ${ProxyOBJ} ${PluginOBJ} ${Onvifobj})
target_link_libraries(onvifcomm crypto avcodec avformat avutil swscale)
add_executable(client main.cpp ${ProxyOBJ} ${PluginOBJ} ${Onvifobj})
target_link_libraries(client ssl crypto avcodec avformat avutil swscale onvifdevice onvifcomm)
#set_target_properties(onvifdevice PROPERTIES OUTPUT_NAME "onvifdevice")
#install(TARGETS onvifdevice DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build)   



