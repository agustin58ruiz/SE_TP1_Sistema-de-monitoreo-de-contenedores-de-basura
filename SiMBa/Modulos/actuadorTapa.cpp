#include  "mbed.h"
#include "actuadorTapa.h"
#include "arm_book_lib.h"
#include "motor.h"

#define MOTOR_PASOS 1024
#define MOTOR_PPM 5000

static Motor motor(D4,D5,D6,D7);

ActuadorTapa::ActuadorTapa(float periodoActualizacion):TrabaTapa(LED1), SensorTapaAbierta(D8, PullDown), SensorTapaCerrada(D9, PullDown){
  TrabaTapa = OFF;  
  motor.EstablecerPPMPorPaso(MOTOR_PPM);
  _actualizacionTicker = new Ticker();
  _actualizacionTicker->attach(callback(this, &ActuadorTapa::_actualizacionCallback), periodoActualizacion);
}

void ActuadorTapa::TrabarTapa(){
    CerrarTapa();
    TrabaTapa = ON;
}

void ActuadorTapa::DestrabarTapa(){
    TrabaTapa = OFF;
}

void ActuadorTapa::AbrirTapa() {
    printf("Abrir Tapa\r\n");
    motor.Avanzar();
    EstadoDeTapa = EstadoTapa::ABRIENDO_TAPA;
}

void ActuadorTapa::CerrarTapa() {
    
    printf("Cerrar Tapa\r\n");
    motor.Retroceder();
    EstadoDeTapa = EstadoTapa::CERRANDO_TAPA;
}

void ActuadorTapa::ActualizarEstado(){
    
    if ( EstadoDeTapa == EstadoTapa::ABRIENDO_TAPA ) {
        if ( SensorTapaAbierta == ON ) {
            motor.Pausar();
            EstadoDeTapa = EstadoTapa::TAPA_ABIERTA;
        }
    } else if ( EstadoDeTapa == EstadoTapa::CERRANDO_TAPA ) {
        if ( SensorTapaCerrada == ON ) {
            motor.Pausar();
            EstadoDeTapa = EstadoTapa::TAPA_CERRADA;
        }
    } else if ( TrabaTapa == ON ) {

        EstadoDeTapa = EstadoTapa::TAPA_TRABADA;
        
    }

}

EstadoTapa ActuadorTapa::Estado() {
    return EstadoDeTapa;
}

void ActuadorTapa::_actualizacionCallback() {
    ActualizarEstado();
}

    