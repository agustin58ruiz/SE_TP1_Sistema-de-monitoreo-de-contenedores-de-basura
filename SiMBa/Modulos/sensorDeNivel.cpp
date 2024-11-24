#include "mbed.h"
#include "arm_book_lib.h"
#include "sensorDeNivel.h"


SensorDeNivel::SensorDeNivel(PinName pin, PinMode mode) : Desborde(pin, mode), EstadoDelContenedor(EstadoContenedor::CONTENEDOR_CON_ESPACIO) {  
}
SensorDeNivel::SensorDeNivel(): Desborde(D1, PullDown), EstadoDelContenedor(EstadoContenedor::CONTENEDOR_CON_ESPACIO) {
}

bool SensorDeNivel::EstaDesbordado(){
    if ( EstadoDelContenedor == EstadoContenedor::CONTENEDOR_LLENO ) {
        return true;
    } else {
        return false;
    }
}

EstadoContenedor SensorDeNivel::Estado() {
    return EstadoDelContenedor;
}

void SensorDeNivel::ActualizarEstado() {
    if ( Desborde == ON ) {
        EstadoDelContenedor = EstadoContenedor::CONTENEDOR_LLENO;
    }else if( Desborde == OFF ) {
        EstadoDelContenedor = EstadoContenedor::CONTENEDOR_CON_ESPACIO;
    }
}