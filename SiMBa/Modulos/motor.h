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
    int PasosRestantes();
    void Pasos(int cantidadDePasos);
};


#endif