#ifndef WI_FI_COMMANDS__H
#define WI_FI_COMMANDS__H
#include "wi-fi-constants.h"
#include "wi-fi-types.h"
#include "uart.h"
#include "selector.h"
enum class WiFiCommandState {
    OK,       // Command run succesfully
    BUSY,
    ERROR_CIPSEND_LINK_IS_NOT_VALID,
    ERROR_CIPMUX_LINK_IS_BUILDED,
    UNKNOWN
};


class WiFiCommands {
private:
    WiFiCommandState _state;
    int _strIndex;
    int _stepIndex;
    Selector * _selector;
    
    ParseStatus _parseOk( Uart& serial, const char* command );
    ParseStatus _waitJustOk( Uart& serial );
    ParseStatus _waitForString( Uart& serial, const char* str );
    ParseStatus _waitForAnswer( Uart& serial );
    ParseStatus _waitColonAndGetString( Uart& serial, char * stringOut, size_t maxStringOut );
    

public:
    WiFiCommands();

    ~WiFiCommands();

    void Reset();

    ParseStatus ParseAtAsync( Uart& serial );

    ParseStatus ParseCipmux( Uart& serial );
    ParseStatus ParseCipmuxAsync( Uart& serial );
    
    ParseStatus ParseCipServer( Uart& serial );
    ParseStatus ParseCipServerAsync( Uart& serial );

    ParseStatus ParseCipRecvData( Uart& serial , char * buf, size_t bufLen);
    ParseStatus ParseCipRecvDataAsync( Uart& serial );

    ParseStatus ParseCipRecvDataAsync( Uart& serial, char * stringOut, size_t maxStrLen);

    ParseStatus ParseCipSendAsync( Uart& serial );
    ParseStatus WaitForCipSendOkAsync( Uart& serial );

    ParseStatus WaitForOkAsync( Uart& serial );

    ParseStatus ParseCipRecvMode( Uart& serial );

    ParseStatus ParseCipCloseAsync( Uart& serial);

    /*void FlushCommandResponse(Uart& serial);*/

    WiFiCommandState GetState();
};
#endif