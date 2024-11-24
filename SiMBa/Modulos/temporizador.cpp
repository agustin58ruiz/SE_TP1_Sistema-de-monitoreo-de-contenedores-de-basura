#include "temporizador.h"
#include "mbed.h"

Temporizador::Temporizador(float segundos){
    _ticker = new Ticker();
    _estado = EstadoTemporizador::NO_EMPEZADO;
}

void Temporizador::Empezar(){
    _ticker->attach(callback(this, &Temporizador::_callback), segundos);
    _estado = EstadoTemporizador::EMPEZADO;
}

void Temporizador::Parar(){
    _estado = EstadoTemporizador::FINALIZADO;
    _ticker->detach();
}

void Temporizador::_callback(){
    Parar();
}