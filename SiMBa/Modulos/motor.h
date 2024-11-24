#ifndef MOTOR__H
#define MOTOR__H

#include "mbed.h"

class Motor {

private:

BusOut* _control;
int _indice;
float _tiempoEntreBobina;

int _cantidadDePasosRestantes;
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
void _pasosCallbackRetroceder();
void _pasosCallbackAvanzar();

public:

Motor(PinName Bit0, PinName Bit1, PinName Bit2, PinName Bit3);

void EstablecerPPMPorPaso(unsigned int rpm);

void Pausar();
void Empezar();
void Parar();
//void Pasos(int cantidadDePasos);
int PasosRestantes();


void Pasos(int cantidadDePasos);
};


#endif