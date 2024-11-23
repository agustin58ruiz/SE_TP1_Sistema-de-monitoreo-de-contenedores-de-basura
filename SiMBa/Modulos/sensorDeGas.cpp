#include "sensorDeGas.h"


SensorDeGas::SensorDeGas(PinName pin): sensor(pin),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO), Umbral(0.5){
}

SensorDeGas::SensorDeGas(): sensor(A0),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO),Umbral(0.5){}

SensorDeGas::SensorDeGas(float limite): sensor(A0),EstadoDeGas(EstadoSensorDeGas::DESCONOCIDO),Umbral(limite){}


EstadoSensorDeGas SensorDeGas::Estado(){
    int nivelDeGas = (int)(sensor.read()*100);
    
    printf("%d\n", nivelDeGas);
    return EstadoDeGas;
}

void SensorDeGas::ActualizarEstado(){
    if(sensor > Umbral){
        EstadoDeGas = EstadoSensorDeGas::SATURADO;
    }else{
        EstadoDeGas = EstadoSensorDeGas::NORMAL;
    }
}