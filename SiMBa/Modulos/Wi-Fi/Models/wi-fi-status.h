#ifndef WI_FI_STATUS__H
#define WI_FI_STATUS__H
#include "wi-fi-constants.h"
#include "uart.h"
#include "tcp-ip-link.h"
#include "link-queue.h"


enum class WiFiConnectionState {
    NOT_STARTED = 0,       // ESP station has not started any Wi-Fi connection
    CONNECTED_NO_IP = 1,   // ESP station has connected to an AP, but does not get an IPv4 address yet
    CONNECTED_WITH_IP = 2, // ESP station has connected to an AP, and got an IPv4 address
    CONNECTING = 3,        // ESP station is in Wi-Fi connecting or reconnecting state
    DISCONNECTED = 4,       // ESP station is in Wi-Fi disconnected state
    UNKNOWN = 5,
};



class WiFiStatus {
private: 
    int _strIndex;
    int _stepIndex;
    int _number;
    WiFiConnectionState _state;
    TcpIpLink** _tcpIpLinks;
    LinkQueue * _linkQueue;
    char _ip[IP_MAX_LENGTH];
    char _mac[MAC_MAX_LENGTH]; 

    

public:
    WiFiStatus();
    ParseStatus Parse( Uart& serial );
    ParseStatus ParseAsync( Uart& serial );
    ParseStatus ParseCipRecvLen( Uart& serial );
    ParseStatus ParseCipRecvLenAsync( Uart& serial );

    ParseStatus ParseCifsrAsync( Uart& serial );

    int GetLink();
    int GetLinkLenght(int link);
    void ResetIndex();
    char * Ip();
    char * Mac();
    WiFiConnectionState GetState() const;
};

#endif
