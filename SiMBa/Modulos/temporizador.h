#ifndef TEMPORIZADOR__H
#define TEMPORIZADOR__H

#include "mbed.h"

enum class EstadoTemporizador{EMPEZADO, NO_EMPEZADO, FINALIZADO};
class Temporizador{

    private:
        EstadoTemporizador _estado;
        Ticker * _ticker;
        float _segundosTemporizador;
        void _callback();
    public:
        Temporizador(float segundos);
        EstadoTemporizador Estado();
        void Empezar();
        
        void Parar();

};


#endif