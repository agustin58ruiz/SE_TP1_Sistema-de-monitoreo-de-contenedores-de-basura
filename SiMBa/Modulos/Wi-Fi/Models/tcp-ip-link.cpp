#include "tcp-ip-link.h"
#include "string.h"
#include "stdlib.h"
#include "uart.h"
#include "at.h"
#include "wi-fi-types.h"

LinkId parseLinkId( const char* token );
Type parseType( const char* token );
TeType parseTeType( const char* token );

TcpIpLink::TcpIpLink( int linkId ) {
    Clear();
    _linkId = (LinkId)linkId;
    _type = Type::UNDEFINED;
    _remoteAddr[0] = '\0';
    _remotePort = 0;
    _localPort = 0;
    _tetype = TeType::UNDEFINED;
    _linkLength = 0;
}

void TcpIpLink::Clear() {
    _linkId = LinkId::UNDEFINED;
    _type = Type::UNDEFINED;
    _remoteAddr[0] = '\0';
    _remotePort = 0;
    _localPort = 0;
    _tetype = TeType::UNDEFINED;
    _linkLength = 0;
}

void TcpIpLink::SetLength(int length) {
    
    _linkLength = length;
}

ParseStatus TcpIpLink::Parse( Uart& serial ) {
    ParseStatus st;
 
    int optIndex=0;
    const char *linkType[6] = {"TCP", "TCPv6", "UDP" ,"UDPv6","SSL" ,"SSLv6"};
    int teType;
    
    if ( (st = parse_string_option ( serial, linkType, 6, &optIndex )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    _type = (Type)optIndex;
    
    if ( (st = wait_for_char(serial, ',')) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    if ( (st = parse_string( serial, _remoteAddr, IPV6_MAX_STRING_CHARS+1 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    if ( (st = wait_for_char(serial, ',')) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    if ( (st = parse_integer_number(serial, &_remotePort )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    if ( (st = parse_integer_number(serial, &_localPort )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    if ( (st = parse_integer_number(serial, &teType)) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    _tetype = (TeType)teType;
    return ParseStatus::SUCCESSFUL_MATCH;
}

int TcpIpLink::GetLength(){
    return _linkLength;
}
