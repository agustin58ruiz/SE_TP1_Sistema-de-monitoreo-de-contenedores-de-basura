#include "wi-fi-status.h"
#include "stdlib.h"
#include "string.h"
#include "at.h"
#include "tcp-ip-link.h"
#include "wi-fi-types.h"

WiFiStatus::WiFiStatus() {
    _state = WiFiConnectionState::UNKNOWN;
    _tcpIpLinks = nullptr;
}

ParseStatus WiFiStatus::Parse(Uart& serial) {
   
    int stateCode = -1; 
    ParseStatus st;

    if ( (st = wait_for_at_echo( serial, "CIPSTATUS\r\n", 11 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    if ( (st = wait_for_string( serial, "STATUS:", 7 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    if ( (st = parse_integer_number( serial, &stateCode )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; } 
   
    switch (stateCode) {
        case 0: _state = WiFiConnectionState::NOT_STARTED; break;
        case 1: _state = WiFiConnectionState::CONNECTED_NO_IP; break;
        case 2: _state = WiFiConnectionState::CONNECTED_WITH_IP; break;
        case 3: _state = WiFiConnectionState::CONNECTING; break;
        case 4: _state = WiFiConnectionState::DISCONNECTED; break;
        default: _state = WiFiConnectionState::UNKNOWN; return ParseStatus::SUCCESSFUL_NO_MATCH;
    }
    
    bool parseConnectionStatuses = true;

    while ( parseConnectionStatuses ) {

        switch ( wait_for_char (serial, '+') ) {
            case ParseStatus::SUCCESSFUL_MATCH: break;
            case ParseStatus::SUCCESSFUL_NO_MATCH: parseConnectionStatuses = false; break;
            case ParseStatus::FAILED_NO_READ: return ParseStatus::FAILED_NO_READ; 
            default: break;
        }
        
        if ( parseConnectionStatuses ) {
            if ( _tcpIpLinks == nullptr ) { _tcpIpLinks = new TcpIpLink*[TCP_IP_MAX_LINKS]; }
            
            int linkId;
            
            if ( (st=parse_integer_number(serial, &linkId)) != ParseStatus::SUCCESSFUL_MATCH ) { return st; } 

            if ( _tcpIpLinks[linkId] == nullptr ) { _tcpIpLinks[linkId] = new TcpIpLink(linkId); }

            if ( (st=_tcpIpLinks[linkId]->Parse( serial )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
            
        }
    }
    if ( (st = wait_for_string( serial, "\nOK\r\n" , 6 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    return ParseStatus::SUCCESSFUL_MATCH;
}



    WiFiConnectionState WiFiStatus::GetState() const {
        return _state;
    }
