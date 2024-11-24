#include "temporizador.h"
#include "mbed.h"

Temporizador::Temporizador(float segundosTemporizador) {
    _ticker = new Ticker();
    _estado = EstadoTemporizador::NO_EMPEZADO;
    _segundosTemporizador = segundosTemporizador;
}

void Temporizador::Empezar() {
    if ( _estado != EstadoTemporizador::EMPEZADO ) {
        _ticker->attach(callback(this, &Temporizador::_callback), _segundosTemporizador);
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