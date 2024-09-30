#include "mbed.h"
#include "actuadorTapa.h"
#include "sensorDeNivel.h"
#include "sensorDeGas.h"
#include "maquinaDeEstados.h"

    ActuadorTapa tapa ;
    SensorDeNivel capacidad;
    SensorDeGas sensorDeGas;
// main() runs in its own thread in the OS

int main()
{
    

    // ------------- DEFINICION DE ESTADOS --------------
    Estado TapaTrabada = Estado((char *)"TAPA_TRABADA");
    Estado TapaDestrabada = Estado((char *)"TAPA_DESTRABADA");

    // ---------- CREACION MAQUINA DE ESTADOS -----------

    MaquinaDeEstados maquina = MaquinaDeEstados(&TapaDestrabada);
    
    // ---------------- TRANSICIONES---------------------

    Transicion TapaTrabada2TapaDestrabada = Transicion(& TapaDestrabada);
    Transicion TapaDestrabada2TapaTrabada = Transicion(& TapaTrabada);
    
    // TRANSICIONES TAPA DESTRABADA
    // TapaDestrabada -> TapaTrabada
    TransicionesPtr_t transicionesTapaDestrabada = new Transicion*[2]{&TapaDestrabada2TapaTrabada, nullptr};
    TapaDestrabada2TapaTrabada.EstablecerCondicion([](){
        sensorDeGas.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();
        
        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO || sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO;

    }).EstablecerAccion([](){
        tapa.TrabarTapa();
    });
    // TRANSICIONES TAPA TRABADA
    // TapaTrabada -> TapaDestrabada

    TransicionesPtr_t transicionesTapaTrabada = new Transicion*[2]{&TapaTrabada2TapaDestrabada, nullptr};
    TapaTrabada2TapaDestrabada.EstablecerCondicion([](){
        
        sensorDeGas.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();

        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_CON_ESPACIO; 

    }).EstablecerAccion([](){
        tapa.DestrabarTapa();
    });


    // ------- Establecer Tranciciones ----------
    TapaTrabada.EstablecerTransiciones(transicionesTapaTrabada);
    TapaDestrabada.EstablecerTransiciones(transicionesTapaDestrabada);
    
    while (true) {

        maquina.Evaluar();
        maquina.ObtenerInformacion();

        /*
        sensorDeGas.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();

        if(capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO || sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO){
            tapa.TrabarTapa();
        }else if( capacidad.Estado() == EstadoContenedor::CONTENEDOR_CON_ESPACIO){
            tapa.DestrabarTapa();
        }

*/
    }
}
/*
void definirTransiciones(void){

    
    

    // ------------- DEFINICION DE ESTADOS --------------
    Estado TapaTrabada = Estado((char *)"TAPA_TRABADA");
    Estado TapaDestrabada = Estado((char *)"TAPA_DESTRABADA");

    // ---------- CREACION MAQUINA DE ESTADOS -----------

    MaquinaDeEstados maquina = MaquinaDeEstados(&TapaDestrabada);
    
    // ---------------- TRANSICIONES---------------------

    Transicion TapaTrabada2TapaDestrabada = Transicion(& TapaDestrabada);
    Transicion TapaDestrabada2TapaTrabada = Transicion(& TapaTrabada);

    // TapaTrabada -> TapaDestrabada
    TapaDestrabada2TapaTrabada.EstablecerCondicion([](){
        sensorDeGas.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();
        
        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO || sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO;

    }).EstablecerAccion([](){
        tapa.TrabarTapa();
    });

    // TapaDestrabada -> TapaTrabada
    TapaDestrabada2TapaTrabada.EstablecerCondicion([](){
        
        sensorDeGas.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();

        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_CON_ESPACIO; 

    }).EstablecerAccion([](){
        tapa.DestrabarTapa();
    });

    



}*/