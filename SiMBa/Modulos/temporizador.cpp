#include "temporizador.h"
#include "mbed.h"

Temporizador::Temporizador() {
    _ticker = new Ticker();
    _estado = EstadoTemporizador::NO_EMPEZADO;
    
}

void Temporizador::Empezar(float segundos) {
    
    if ( _estado != EstadoTemporizador::EMPEZADO ) {
        _ticker->attach(callback(this, &Temporizador::_callback), segundos);
        _estado = EstadoTemporizador::EMPEZADO;
    }
}

void Temporizador::Parar() {
    _estado = EstadoTemporizador::FINALIZADO;
    _ticker->detach();
}

void Temporizador::_callback() {
    Parar();
}

EstadoTemporizador Temporizador::Estado() {
    return _estado;
}