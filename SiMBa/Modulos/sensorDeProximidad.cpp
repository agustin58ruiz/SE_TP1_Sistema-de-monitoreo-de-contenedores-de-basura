#include "mbed.h"
#include "arm_book_lib.h"
#include "sensorDeProximidad.h"


SensorDeProximidad::SensorDeProximidad(PinName pin, PinMode mode) : Pir(pin, mode), EstadoPresenciaUsuario(EstadoPresencia::USUARIO_NO_DETECTADO)
{  
}




bool SensorDeProximidad::HayUsuario(){
    if( EstadoPresenciaUsuario == EstadoPresencia::USUARIO_DETECTADO ){

        return true;
    }
    else{
        return false;
    }
}

EstadoPresencia SensorDeProximidad::Estado(){
    return EstadoPresenciaUsuario;

}

void SensorDeProximidad::ActualizarEstado(){
    
    if (Pir == ON){
        printf("Hay persona\n");
        EstadoPresenciaUsuario = EstadoPresencia::USUARIO_DETECTADO;
    }else if(Pir == OFF){
        printf("NO Hay persona\n");
        EstadoPresenciaUsuario = EstadoPresencia::USUARIO_NO_DETECTADO;
    }
}