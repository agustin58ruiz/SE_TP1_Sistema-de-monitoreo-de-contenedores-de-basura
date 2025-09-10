#include "sensorDeTemperatura.h"

SensorDeTemperatura::SensorDeTemperatura(PinName pin): 
    sensor(pin),
    EstadoDeTemperatura(EstadoSensorDeTemperatura::DESCONOCIDO), 
    Umbral(0.5),
    temperatura(0) {
}

SensorDeTemperatura::SensorDeTemperatura(): 
    sensor(A1),
    EstadoDeTemperatura(EstadoSensorDeTemperatura::DESCONOCIDO),
    Umbral(0.5),
    temperatura(0) {
}

SensorDeTemperatura::SensorDeTemperatura(int limiteDeTemperatura): 
    sensor(A1),
    EstadoDeTemperatura(EstadoSensorDeTemperatura::DESCONOCIDO),
    Umbral(limiteDeTemperatura),
    temperatura(0) {
}

EstadoSensorDeTemperatura SensorDeTemperatura::Estado() {
    printf("Temperatura: %d\n", (int)temperatura);
    return EstadoDeTemperatura;
}

void SensorDeTemperatura::ActualizarEstado() {
    float valor_actual;
    
    valor_actual = sensor.read()*66;

    if(valor_actual == 0 ) { valor_actual = temperatura;}
    
    
    temperatura = 0.1*valor_actual+0.9*temperatura;

    //temperatura = (int)(sensor.read()*66);

    if(temperatura > Umbral){
        EstadoDeTemperatura = EstadoSensorDeTemperatura::CALIENTE;
    }else{
        EstadoDeTemperatura = EstadoSensorDeTemperatura::NORMAL;
    }
}

void SensorDeTemperatura::ObtenerTemperatura(char * tempString) {
    sprintf( tempString, "%doC", (int)temperatura );
}

void SensorDeTemperatura::ObtenerTemperaturaNumero(char * tempString) {
    sprintf( tempString, "%d", (int)temperatura );
}
