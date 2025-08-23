#ifndef WI_FI__H
#define WI_FI__H

#include "maquinaDeEstados.h"
#include "temporizador.h"
#include "app.h"
#include "mbed.h"
#include "link-queue.h"

//=====[Declaration of public defines]=========================================

#define AP_SSID_MAX_LENGTH     (32 + 1)
#define AP_PASSWORD_MAX_LENGTH (63 + 1)
#define STRING_2_SEND_MAX_LENGTH 50

class Wifi {
private:
InterruptIn* _rx;

MaquinaDeEstados * _maquina;
Temporizador * _delay;
const char* _comExpectedResponse;
int _strPositionIndex;
char _receivedChar;
int _currentConnectionId;
int _lengthOfHtmlCode;
char _strToSendRecv[STRING_2_SEND_MAX_LENGTH] = "";
char _route[HTTP_MAX_HEADER_CHARS];
bool _logs;

bool _isExpectedResponse();
bool _isOneOfTheseResponses( const char** responses, int* pos );
//bool _comCharRead( char* receivedChar );
//void _comStringRead( char* str );
//void _comStringWrite( const char* str );
void _readCallback();
App* _app;
void _trimRoute();
public:
Wifi(App *app);
void ComInit();
void ComRestart();
void ComUpdate();
void Logs(bool mostrarLogs);
void SendHtml( const char* content );

void SendHtmlN( const char* content, int length);
char * Route();


BufferedSerial* wifi;
};


#endif