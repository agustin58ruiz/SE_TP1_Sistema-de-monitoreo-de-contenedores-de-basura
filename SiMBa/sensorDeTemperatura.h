#ifndef SENSOR_DE_TEMPERATURA_H
#define SENSOR_DE_TEMPERATURA_H

#include "mbed.h"
#include "arm_book_lib.h"

enum class EstadoSensorDeTemperatura {NORMAL, CALIENTE, DESCONOCIDO};

class SensorDeTemperatura {

    private:

    AnalogIn sensor;
    EstadoSensorDeTemperatura EstadoDeTemperatura;
    int Umbral;
    int temperatura;
    public: 

    SensorDeTemperatura(PinName pin);
    SensorDeTemperatura();
    SensorDeTemperatura(int limiteDeTemperatura);

    EstadoSensorDeTemperatura Estado();
    void ActualizarEstado();
    void ObtenerTemperatura( char* );
};

#endif