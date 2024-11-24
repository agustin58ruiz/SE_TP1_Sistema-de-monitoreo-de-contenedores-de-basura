#include "mbed.h"
#include "arm_book_lib.h"
#include "sensorDePresencia.h"


SensorDePresencia::SensorDePresencia(PinName pin, PinMode mode) : Pir(pin, mode), EstadoPresenciaUsuario(EstadoPresencia::USUARIO_NO_DETECTADO) {
    _pir = new InterruptIn(pin, mode);
    _pir->fall( callback( this, &SensorDePresencia::_usuarioNoDetectadoCallback ) ); 
    _pir->rise( callback( this, &SensorDePresencia::_usuarioDetectadoCallback ) );
}

bool SensorDePresencia::HayUsuario(){
    if ( EstadoPresenciaUsuario == EstadoPresencia::USUARIO_DETECTADO ) {
        return true;
    } else {
        return false;
    }
}

EstadoPresencia SensorDePresencia::Estado(){
    return EstadoPresenciaUsuario;
}

void SensorDePresencia::ActualizarEstado() {
    if ( Pir == ON ) {
        EstadoPresenciaUsuario = EstadoPresencia::USUARIO_DETECTADO;
    }else if ( Pir == OFF ) {
        EstadoPresenciaUsuario = EstadoPresencia::USUARIO_NO_DETECTADO;
    }
}

void SensorDePresencia::_usuarioDetectadoCallback() {
    EstadoPresenciaUsuario = EstadoPresencia::USUARIO_DETECTADO;
}
void SensorDePresencia::_usuarioNoDetectadoCallback() {
    EstadoPresenciaUsuario = EstadoPresencia::USUARIO_NO_DETECTADO;
}