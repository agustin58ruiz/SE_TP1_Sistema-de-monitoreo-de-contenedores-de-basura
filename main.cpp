#include "mbed.h"
#include "actuadorTapa.h"
#include "sensorDeNivel.h"
// main() runs in its own thread in the OS
int main()
{

    ActuadorTapa tapa ;
    SensorDeNivel capacidad;
    
    while (true) {
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();

        if(capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO){
            tapa.TrabarTapa();
        }else if( capacidad.Estado() == EstadoContenedor::CONTENEDOR_CON_ESPACIO){
            tapa.DestrabarTapa();
        }


    }
}