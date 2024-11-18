#ifndef MOTOR__H
#define MOTOR__H

#include "mbed.h"

class Motor {

private:

BusOut* _control;
int _indice;
unsigned int _tiempoEntreBobina;

int _cantidadDePasos;
int _pasos;

bool _revertir;
InterruptIn * _revertirInt;
bool _funcionando;

Ticker * _tickerMotor;

void _retrocederUnPaso();
void _avanzarUnPaso();
void _revertirCallback();
void _pasosCallback();

public:

Motor(PinName Bit0, PinName Bit1, PinName Bit2,PinName Bit3);

void EstablecerRPMPorPaso(unsigned int rpm);
void DesactivarInterrupcionRevertir();
void ActivarInterrupcionRevertir();
void EstablecerInterrupcionRevertir(PinName pin, PinMode mode);
void Pasos(int cantidadDePasos);
void PasosNew(int cantidadDePasos);
};


#endif