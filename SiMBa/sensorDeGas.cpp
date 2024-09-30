#include "sensorDeGas.h"


SensorDeGas::SensorDeGas(PinName pin): sensor(pin),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO), umbral(0.5){
}

SensorDeGas::SensorDeGas(): sensor(A0),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO),umbral(0.5){}

SensorDeGas::SensorDeGas(float limite): sensor(A0),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO),umbral(limite){}


EstadoSensorDeGas SensorDeGas::Estado(){
    return EstadoDeGas;
}

void SensorDeGas::ActualizarEstado(){
    if(sensor > umbral){
        EstadoDeGas = EstadoSensorDeGas::SATURADO;
    }else{
        EstadoDeGas = EstadoSensorDeGas::NORMAL;
    }
}
