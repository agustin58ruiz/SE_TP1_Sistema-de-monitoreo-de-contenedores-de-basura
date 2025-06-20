#ifndef WI_FI_STATUS__H
#define WI_FI_STATUS__H
#include "wi-fi-constants.h"
#include "uart.h"
#include "tcp-ip-link.h"


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
    WiFiConnectionState _state;
    TcpIpLink** _tcpIpLinks;
public:
    WiFiStatus();
    ParseStatus Parse( Uart& serial );
    WiFiConnectionState GetState() const;
};

#endif
