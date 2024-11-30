#include "sensorDeTemperatura.h"

SensorDeTemperatura::SensorDeTemperatura(PinName pin): sensor(pin),EstadoDeTemperatura(EstadoSensorDeTemperatura::DESCONOCIDO), Umbral(0.5) {
}

SensorDeTemperatura::SensorDeTemperatura(): sensor(A1),EstadoDeTemperatura(EstadoSensorDeTemperatura::DESCONOCIDO),Umbral(0.5) {
}

SensorDeTemperatura::SensorDeTemperatura(int limiteDeTemperatura): sensor(A1),EstadoDeTemperatura(EstadoSensorDeTemperatura::DESCONOCIDO),Umbral(limiteDeTemperatura) {
}

EstadoSensorDeTemperatura SensorDeTemperatura::Estado() {
    printf("Temperatura: %d\n", temperatura);
    return EstadoDeTemperatura;
}

void SensorDeTemperatura::ActualizarEstado() {
    temperatura = (int)(sensor.read()*66);
    if(temperatura > Umbral){
        EstadoDeTemperatura = EstadoSensorDeTemperatura::CALIENTE;
    }else{
        EstadoDeTemperatura = EstadoSensorDeTemperatura::NORMAL;
    }
}

void SensorDeTemperatura::ObtenerTemperatura(char * tempString) {
    sprintf( tempString, "%doC", temperatura );
}
