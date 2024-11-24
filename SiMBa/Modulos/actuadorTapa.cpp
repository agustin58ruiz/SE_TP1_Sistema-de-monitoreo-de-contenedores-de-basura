#include  "mbed.h"
#include "actuadorTapa.h"
#include "arm_book_lib.h"

ActuadorTapa::ActuadorTapa():TrabaTapa(LED1){
  TrabaTapa = OFF;  
}

void ActuadorTapa::TrabarTapa(){
    TrabaTapa = ON;
}
void ActuadorTapa::DestrabarTapa(){
    TrabaTapa = OFF;
}

void ActuadorTapa::ActualizarEstado(){
    if(TrabaTapa == ON){
        EstadoDeTapa = EstadoTapa::TAPA_TRABADA;
    }else if(TrabaTapa == OFF){
        EstadoDeTapa = EstadoTapa::TAPA_DESTRABADA;
    }
}

EstadoTapa ActuadorTapa::Estado(){
    return EstadoDeTapa;
}