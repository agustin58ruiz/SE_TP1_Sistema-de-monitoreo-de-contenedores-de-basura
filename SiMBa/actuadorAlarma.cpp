#include  "mbed.h"
#include "actuadorAlarma.h"
#include "arm_book_lib.h"

ActuadorAlarma::ActuadorAlarma():ActuadorDeAlarma(D2)
{
  ActuadorDeAlarma = OFF;  
}

void ActuadorAlarma::Activar(){
    ActuadorDeAlarma = ON;
}
void ActuadorAlarma::Desactivar(){
    ActuadorDeAlarma = OFF;
}

void ActuadorAlarma::ActualizarEstado(){
    if(ActuadorDeAlarma == ON){
        EstadoDeLaAlarma = EstadoAlarma::ALARMA_ACTIVADA;
    }else if(ActuadorDeAlarma == OFF){
        EstadoDeLaAlarma = EstadoAlarma::ALARMA_DESACTIVADA;
    }
}

EstadoAlarma ActuadorAlarma::Estado(){
    return EstadoDeLaAlarma;
}