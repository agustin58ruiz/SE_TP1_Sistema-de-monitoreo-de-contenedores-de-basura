#ifndef WI_FI__H
#define WI_FI__H

#include "maquinaDeEstados.h"
#include "temporizador.h"

#include "mbed.h"

//=====[Declaration of public defines]=========================================

#define AP_SSID_MAX_LENGTH     (32 + 1)
#define AP_PASSWORD_MAX_LENGTH (63 + 1)

class Wifi {
private:
MaquinaDeEstados * _maquina;
Temporizador * _delay;
const char* _comExpectedResponse;
int _ipStringPositionIndex;
char _receivedChar;

bool _isExpectedResponse();
bool _comCharRead( char* receivedChar );
void _comStringWrite( const char* str );
 

public:
Wifi();
void ComInit();
void ComRestart();
void ComUpdate();

UnbufferedSerial* wifi;
};


#endif