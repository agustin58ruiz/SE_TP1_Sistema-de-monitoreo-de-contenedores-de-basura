#ifndef LINK__H
#define LINK__H
#include "wi-fi-constants.h"
#include "wi-fi-types.h"
#include "uart.h"

enum class LinkId { LINK_0 = 0, LINK_1, LINK_2, LINK_3, LINK_4, UNDEFINED};
enum class Type { TCP, TCPv6, UDP, UDPv6, SSL, SSLv6, UNDEFINED };
enum class TeType { ESP_RUNS_AS_A_CLIENT, ESP_RUNS_AS_A_SERVER, UNDEFINED };


class TcpIpLink {
private:
    LinkId _linkId;
    Type _type;
    char _remoteAddr[IPV6_MAX_STRING_CHARS+1];
    int _remotePort;
    int _localPort;
    TeType _tetype;


public: 
    TcpIpLink(int linkId);
    
    ParseStatus Parse(Uart &serial);

    void Clear();

};

#endif