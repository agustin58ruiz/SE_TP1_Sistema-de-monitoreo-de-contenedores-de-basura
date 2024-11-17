#ifndef MOTOR__H
#define MOTOR__H

#include "mbed.h"

class Motor {

private:

BusOut* _control;
int _indice;

public:

Motor(PinName Bit0, PinName Bit1, PinName Bit2,PinName Bit3);
void Avanzar();
void Retroceder();
};

#endif