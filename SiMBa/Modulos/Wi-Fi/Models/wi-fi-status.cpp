#include "wi-fi-status.h"
#include "stdlib.h"
#include "string.h"
#include "at.h"
#include "tcp-ip-link.h"
#include "wi-fi-types.h"
#include "link-queue.h"

WiFiStatus::WiFiStatus() {
    _state = WiFiConnectionState::UNKNOWN;
    _strIndex = 0;
    _stepIndex = 0;
    _number = 0;
    _tcpIpLinks = nullptr;
    _linkQueue = new LinkQueue();
}

void WiFiStatus::ResetIndex() {
    _strIndex = 0; 
    _stepIndex = 0;
    _number = 0;

}

ParseStatus WiFiStatus::Parse( Uart& serial) {
   
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
            if ( _tcpIpLinks == nullptr ) { 
                _tcpIpLinks = new TcpIpLink*[TCP_IP_MAX_LINKS]; 

                for (int i = 0; i < TCP_IP_MAX_LINKS; i++) {
                    _tcpIpLinks[i] = nullptr; 
                }    
            }
            
            int linkId;
            if ( (st = wait_for_string( serial, "CIPSTATUS:", 10 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
      
            if ( (st=parse_integer_number(serial, &linkId)) != ParseStatus::SUCCESSFUL_MATCH ) { return st; } 
            
            _linkQueue->Enqueue(linkId);
            

            if ( _tcpIpLinks[linkId] == nullptr ) { _tcpIpLinks[linkId] = new TcpIpLink(linkId); }
            
            if ( (st=_tcpIpLinks[linkId]->Parse( serial )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
      
        }
    }
    
    if ( (st = wait_for_string( serial, "\nOK\r\n" , 6 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    return ParseStatus::SUCCESSFUL_MATCH;
}

ParseStatus WiFiStatus::ParseAsync( Uart& serial) {
   
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
/*0*/   [ &serial, this](){ 
    
            _linkQueue->Reset();
            return wait_for_string_async( serial, _strIndex,"CIPSTATUS\r\nSTATUS:" ); },
/*1*/   [ &serial, this](){
            
            _number = 0;
            return ParseStatus::SUCCESSFUL_MATCH; },
/*2*/   [ &serial, this](){
            ParseStatus st;

            if ( (st = parse_integer_number_async( serial, _number)) == ParseStatus::SUCCESSFUL_MATCH ) {
                switch (_number) {
                    case 0: _state = WiFiConnectionState::NOT_STARTED; break;
                    case 1: _state = WiFiConnectionState::CONNECTED_NO_IP; break;
                    case 2: _state = WiFiConnectionState::CONNECTED_WITH_IP; break;
                    case 3: _state = WiFiConnectionState::CONNECTING; break;
                    case 4: _state = WiFiConnectionState::DISCONNECTED; break;
                    default: _state = WiFiConnectionState::UNKNOWN; return ParseStatus::SUCCESSFUL_NO_MATCH;
                }
                
            } 

            return st;
        },
/*3*/   [ &serial, this](){ 
            ParseStatus st;
            if((st=wait_for_string_async( serial, _strIndex, "+" )) == ParseStatus::SUCCESSFUL_NO_MATCH) {
                _stepIndex=8; 
                
                st= ParseStatus::SUCCESSFUL_MATCH;
            }

            return st;
        
        },
/*4*/   [ &serial, this]() { 
            if ( _tcpIpLinks == nullptr ) { 
                    _tcpIpLinks = new TcpIpLink*[TCP_IP_MAX_LINKS]; 
    
                    for (int i = 0; i < TCP_IP_MAX_LINKS; i++) {
                        _tcpIpLinks[i] = nullptr; 
                    }    
            }
            _number = 0;
            return ParseStatus::SUCCESSFUL_MATCH;
        },
/*5*/   [ &serial, this]() { return wait_for_string_async( serial,  _strIndex, "CIPSTATUS:"); },
/*6*/   [ &serial, this]() { return  parse_integer_number_async( serial, _number); },
/*7*/   [ &serial, this]() { 
            _linkQueue->Enqueue(_number);
            if ( _tcpIpLinks[_number] == nullptr ) { _tcpIpLinks[_number] = new TcpIpLink(_number); }
            return ParseStatus::SUCCESSFUL_MATCH;
         },
/*8*/   [ &serial, this]() { 
            _stepIndex = 2;
            return _tcpIpLinks[_number]->Parse( serial ); },
/*9*/   [ &serial, this]() { return wait_for_string_async( serial, _strIndex,"OK\r\n" ); },
        
    };
    
   // printf("ESTOY EN EL PASO: %d\r\n", _stepIndex);

    const size_t handlerCount = sizeof(handlers) / sizeof(handlers[0]);
    //printf("Estoy en el paso: %d\r\n", _stepIndex);
    if (  _stepIndex >= handlerCount  ) { 
        _stepIndex = 0; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_MATCH; 
    } else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 

        
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
    
    return st;

}

ParseStatus WiFiStatus::ParseCipRecvLen( Uart& serial ) {
    ParseStatus st;
    int linkLength;
    
    
   
    if ( (st = wait_for_at_echo( serial, "CIPRECVLEN?\r\n", 13 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }

    
    if ( (st = wait_for_string( serial, "+CIPRECVLEN:", 12 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    //223,0,0,0,0
    
    for ( int i = 0; i < TCP_IP_MAX_LINKS; i++ ) {

        if ( !serial.Readable() ) {i--; continue;}
        if ( (st=parse_integer_number(serial, &linkLength)) != ParseStatus::SUCCESSFUL_MATCH ) { return st; } 

        if (  _tcpIpLinks[i] != nullptr ) { _tcpIpLinks[i]->SetLength(linkLength); }

    }
    
    if ( (st = wait_for_string( serial, "\r\nOK\r\n" , 6 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    return ParseStatus::SUCCESSFUL_MATCH;
}

ParseStatus WiFiStatus::ParseCipRecvLenAsync( Uart& serial ) {
    ParseStatus st;
    int partialIndex;
    
    
    
    std::function<ParseStatus()> handlers[] = {
        //[ &serial, this](){ return wait_for_at_echo_async( serial, _strIndex,"CIPRECVLEN?" ); },
        //[ &serial, this](){ return wait_for_string_async( serial, _strIndex, "+CIPRECVLEN:" ); },
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex, ":" ); },
        [ &serial, this](){
            ParseStatus st;
            if ( (st = parse_integer_number_async( serial, _number)) == ParseStatus::SUCCESSFUL_MATCH ) {
                
                if (  _tcpIpLinks[_stepIndex-1] != nullptr ) { _tcpIpLinks[_stepIndex-1]->SetLength(_number); }
                _number = 0; 
            } 
            return st;
        },
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"\r\nOK\r\n" ); },
        nullptr
    };

    for (int i = 0; i < AT_MAX_ITERATION_READ; i++) {
    
    //partialIndex = ( _stepIndex < 1 ) ? _stepIndex : (((_stepIndex - 1) <  TCP_IP_MAX_LINKS)? 1 : (_stepIndex) - TCP_IP_MAX_LINKS  );

    if ( _stepIndex < 1 ) {
        partialIndex = _stepIndex;
    } else if ( (_stepIndex - 1) <  TCP_IP_MAX_LINKS ) {
        partialIndex = 1;
    } else partialIndex = (_stepIndex+1) - TCP_IP_MAX_LINKS;



    if ( handlers[partialIndex] == nullptr ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[partialIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    if ( st == ParseStatus::SUCCESSFUL_MATCH || st == ParseStatus::FAILED_NO_READ) {
            break;
        }
    }
    return st;
    
}


    WiFiConnectionState WiFiStatus::GetState() const {
        return _state;
    }

int WiFiStatus::GetLink(){
    return _linkQueue->Dequeue();
}

int WiFiStatus::GetLinkLenght(int link) {

    int len;

    if (link < 0 || link > 4) {
        len = 0;
    }
    else if ( _tcpIpLinks == nullptr ) { len = 0; } 
    else if (_tcpIpLinks[link] == nullptr ) { len = 0;}
    else len = _tcpIpLinks[link]->GetLength();

    return len;
}

ParseStatus WiFiStatus::ParseCifsrAsync( Uart& serial ) {
    
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"+CIFSR:STAIP," ); },
        [ &serial, this]() { return parse_string_async(serial, _strIndex, _ip, IP_MAX_LENGTH); },
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"+CIFSR:STAMAC," ); },
        [ &serial, this]() { return parse_string_async(serial, _strIndex, _mac, MAC_MAX_LENGTH); },
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"OK\r\n" ); },
        
    };
    
    const size_t handlerCount = sizeof(handlers) / sizeof(handlers[0]);

    if (  _stepIndex >= handlerCount  ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
    
    return st;

}


char * WiFiStatus::Ip() {
    return _ip;
}

char * WiFiStatus::Mac() {
    return _mac;
}