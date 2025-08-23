#ifndef ACTUADOR_TAPA_H
#define ACTUADOR_TAPA_H

#include "mbed.h"

enum class EstadoTapa {TAPA_TRABADA, TAPA_ABIERTA, TAPA_CERRADA, CERRANDO_TAPA, ABRIENDO_TAPA};

class ActuadorTapa
{
    private:
        EstadoTapa EstadoDeTapa;
        DigitalOut TrabaTapa;
        DigitalIn SensorTapaAbierta;
        DigitalIn SensorTapaCerrada;

        Ticker *_actualizacionTicker;
        void _actualizacionCallback();
        
    public: 
        ActuadorTapa(float periodoActualizacion);
        void TrabarTapa();
        void DestrabarTapa();
        void ActualizarEstado();
        void CerrarTapa();
        void AbrirTapa();
        EstadoTapa Estado();
};

#endif