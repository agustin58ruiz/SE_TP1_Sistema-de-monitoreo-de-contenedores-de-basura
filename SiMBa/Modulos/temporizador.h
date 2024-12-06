#ifndef TEMPORIZADOR__H
#define TEMPORIZADOR__H

#include "mbed.h"

enum class EstadoTemporizador{EMPEZADO, NO_EMPEZADO, FINALIZADO};

class Temporizador {
private:
    EstadoTemporizador _estado;
    Ticker * _ticker;
    void _callback();
public:
    Temporizador();
    EstadoTemporizador Estado();
    void Empezar(float segundos);
    void Parar();
};

#endif