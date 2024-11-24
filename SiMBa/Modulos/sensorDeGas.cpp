#include "sensorDeGas.h"
#include "mbed.h"




SensorDeGas::SensorDeGas(PinName pin, float umbral, float periodoActualizacion): sensor(pin),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO), Umbral(umbral), _periodoActualizacion(periodoActualizacion){

    _actualizacionTicker = new Ticker();

    _actualizacionTicker->attach(callback(this, &SensorDeGas::_actualizacionCallback),_periodoActualizacion);

}

EstadoSensorDeGas SensorDeGas::Estado(){
    return EstadoDeGas;
}

void SensorDeGas::ActualizarEstado(){
    if(sensor > Umbral){
        EstadoDeGas = EstadoSensorDeGas::SATURADO;
    }else{
        EstadoDeGas = EstadoSensorDeGas::NORMAL;
    }
}

void SensorDeGas::_actualizacionCallback(){
    ActualizarEstado();
}
