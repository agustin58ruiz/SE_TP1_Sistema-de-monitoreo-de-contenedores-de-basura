#ifndef SENSOR_DE_GAS_H
#define SENSOR_DE_GAS_H

#include "mbed.h"
#include "arm_book_lib.h"

enum class EstadoSensorDeGas {NORMAL, SATURADO, DESCONOCIDO};

class SensorDeGas {

    private:

    AnalogIn sensor;
    EstadoSensorDeGas EstadoDeGas;
    float Umbral;
    public: 

    SensorDeGas(PinName pin);
    SensorDeGas();
    SensorDeGas(float limite);

    EstadoSensorDeGas Estado();
    void ActualizarEstado();




};

#endif