#ifndef ACTUADOR_TAPA_H
#define ACTUADOR_TAPA_H

#include "mbed.h"

enum class EstadoTapa {TAPA_TRABADA, TAPA_DESTRABADA} ;

class ActuadorTapa{
    private:
        EstadoTapa EstadoDeTapa;
        DigitalOut TrabaTapa;
    public: 
        
        ActuadorTapa();
        void TrabarTapa();
        void DestrabarTapa();

        void ActualizarEstado();
        EstadoTapa Estado();


};

#endif
