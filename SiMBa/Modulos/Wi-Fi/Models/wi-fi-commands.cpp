#include "wi-fi-commands.h"
#include "at.h"

static const char* options[] = {"OK\r\n", "ERROR\r\n", nullptr};

WiFiCommands::WiFiCommands() {
    _strIndex = 0; 
    _stepIndex = 0;
    _state = WiFiCommandState::UNKNOWN;
    _selector = new Selector(options);
}

WiFiCommands::~WiFiCommands() {
    
    delete _selector;
}

void WiFiCommands::Reset() {
    _strIndex = 0;
    _stepIndex = 0;
    _selector->Reset();
}

ParseStatus WiFiCommands::_waitForAnswer( Uart& serial ) {
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"\r\n" ); },
        [ &serial, this](){ return wait_for_option_async( serial, _selector ); },
        nullptr
    };
    
    if ( handlers[_stepIndex] == nullptr ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    return st;

}

ParseStatus WiFiCommands::_waitJustOk( Uart& serial ) { 
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"OK\r\n" ); },
        nullptr
    };
    
    if ( handlers[_stepIndex] == nullptr ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    return st;

}

ParseStatus WiFiCommands::_waitColonAndGetString( Uart& serial, char * stringOut, size_t maxStringOut ) { 
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,":" ); },
        [ &serial, stringOut, maxStringOut, this]() { return readline_async(serial, _strIndex, stringOut, maxStringOut); },
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"OK\r\n" ); },
        nullptr
    };
    
    if ( handlers[_stepIndex] == nullptr ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    return st;

}

ParseStatus WiFiCommands::_parseOk( Uart& serial, const char* command ) {
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, command, this](){ return wait_for_at_echo_async( serial, _strIndex, command ); },
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"\r\nOK\r\n" ); },
        nullptr
    };
    
    if ( handlers[_stepIndex] == nullptr ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    return st;
}

ParseStatus WiFiCommands::_waitForString( Uart& serial, const char* str) {
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, str,this](){ return wait_for_string_async( serial, _strIndex, str ); }
    };

    const size_t handlerCount = sizeof(handlers) / sizeof(handlers[0]);

    if ( _stepIndex >= handlerCount ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    return st;
}




ParseStatus WiFiCommands::ParseAtAsync( Uart& serial ) {
    ParseStatus st;
    std::function<ParseStatus()> handlers[] = {
        [ &serial, this](){ return wait_for_string_async( serial, _strIndex,"AT\r\n\r\nOK\r\n" ); },
        nullptr
    };
    
    if ( handlers[_stepIndex] == nullptr ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else if ( (st = handlers[_stepIndex]()) == ParseStatus::SUCCESSFUL_MATCH ) { 
        _stepIndex++; 
        _strIndex = 0; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    }
        
    return st;
}

ParseStatus WiFiCommands::ParseCipmux( Uart& serial ) {
    ParseStatus st;

    if ( (st = wait_for_at_echo( serial, "CIPMUX=1\r\n", 10 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    if ( (st = wait_for_string( serial, "\r\nOK\r\n" , 6 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    _state = WiFiCommandState::OK;
    return st;
}

ParseStatus WiFiCommands::ParseCipmuxAsync( Uart& serial ) {
    ParseStatus st;

    st = _parseOk( serial , "CIPMUX=1\r\n" );

    return st;
}


ParseStatus WiFiCommands::ParseCipRecvData( Uart& serial , char * buf, size_t bufLen) {
    ParseStatus st;
    char str[25];

    sprintf(str,"CIPRECVDATA=,%d:", bufLen);
    
    if ( (st = wait_for_at_echo( serial, str, 16 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    serial.ReadString(buf, bufLen);
    buf[bufLen-1] = '\0';

    return st;
}

ParseStatus WiFiCommands::ParseCipRecvDataAsync( Uart& serial) {
    return _waitJustOk( serial );
}

ParseStatus WiFiCommands::ParseCipRecvDataAsync( Uart& serial, char * stringOut, size_t maxStrLen) {
    ParseStatus st;
    
    for( int i = 0; i < AT_MAX_ITERATION_READ;i++) {
        st = _waitColonAndGetString( serial, stringOut, maxStrLen );

        if ( st == ParseStatus::SUCCESSFUL_MATCH || st == ParseStatus::FULL_BUFFER || st == ParseStatus::FAILED_NO_READ ) {
            break;
        }
    }
    return st;
    
}


ParseStatus WiFiCommands::ParseCipServer( Uart& serial ) {
    ParseStatus st;

    
    if ( (st = wait_for_at_echo( serial, "CIPSERVER=1,80\r\n", 16 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    if ( (st = wait_for_string( serial, "\r\nOK\r\n" , 6 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }

    _state = WiFiCommandState::OK;
    
    return st;
}

ParseStatus WiFiCommands::ParseCipServerAsync( Uart& serial ) {
    ParseStatus st;

    st = _parseOk( serial , "CIPSERVER=1,80\r\nno change\r\n" );

    return st;
}

ParseStatus WiFiCommands::ParseCipSendAsync( Uart& serial ) {
    ParseStatus st;

    for( int i = 0; i < AT_MAX_ITERATION_READ ;i++) {
        st = _waitForString( serial, "OK\r\n> " );

        if ( st == ParseStatus::SUCCESSFUL_MATCH || st == ParseStatus::FAILED_NO_READ) {
            break;
        }
    }
    return st;
}

ParseStatus WiFiCommands::ParseCipRecvMode( Uart& serial ) {
    ParseStatus st;

    
    if ( (st = wait_for_at_echo( serial, "CIPRECVMODE=1\r\n", 15  )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    if ( (st = wait_for_string( serial, "\r\nOK\r\n" , 6 )) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }

    _state = WiFiCommandState::OK;
    
    return st;
}

ParseStatus WiFiCommands::WaitForCipSendOkAsync( Uart& serial ) {
    return _waitForString( serial,  "SEND OK\r\n" );
}

ParseStatus WiFiCommands::WaitForOkAsync( Uart& serial ) {
    ParseStatus st;
    
    
   // for( int i = 0; i < AT_MAX_ITERATION_READ;i++) {
        st = _waitForString( serial,  "OK\r\n" );

    //    if ( st == ParseStatus::SUCCESSFUL_MATCH ) {
    //        break;
    //    }
    //}
    return st;
}

WiFiCommandState WiFiCommands::GetState() {
    return _state;
}

ParseStatus WiFiCommands::ParseCipCloseAsync( Uart& serial) {
    ParseStatus st;

    st = _waitForAnswer(serial);
    if ( st == ParseStatus::SUCCESSFUL_MATCH ) {
       _state = (WiFiCommandState) _selector->Result();
    } else {
        _state = WiFiCommandState::UNKNOWN;
    }

    return st;

}
/*void WiFiCommands::FlushCommandResponse(Uart& serial) {
    flush_last_command_response( Uart& serial );
}*/