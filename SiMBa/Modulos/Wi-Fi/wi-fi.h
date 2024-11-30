#ifndef WI_FI__H
#define WI_FI__H

#include "maquinaDeEstados.h"

class Wifi {
private:
MaquinaDeEstados * _maquina;

public:
Wifi();
void ComInit();
void ComRestart();
void ComUpdate();
}


#endif