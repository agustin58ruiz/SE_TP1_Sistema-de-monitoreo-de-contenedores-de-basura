#ifndef ACTUADOR_ALARMA_H
#define ACTUADOR_ALARMA_H

#include "mbed.h"

enum class EstadoAlarma {ALARMA_ACTIVADA, ALARMA_DESACTIVADA} ;

class ActuadorAlarma
{
    private:
        EstadoAlarma EstadoDeLaAlarma;
        DigitalOut ActuadorDeAlarma;
    public: 
        
        ActuadorAlarma();
        void Activar();
        void Desactivar();

        void ActualizarEstado();
        EstadoAlarma Estado();
};

#endif