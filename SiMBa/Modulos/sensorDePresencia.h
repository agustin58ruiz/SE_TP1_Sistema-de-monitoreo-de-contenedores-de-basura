
#ifndef SENSOR_DE_PROXIMIDAD_H
#define SENSOR_DE_PROXIMIDAD_H

#include "mbed.h"


enum class EstadoPresencia{USUARIO_DETECTADO, USUARIO_NO_DETECTADO};

class SensorDePresencia{
private:
    DigitalIn Pir;
    EstadoPresencia EstadoPresenciaUsuario; 
    InterruptIn * _pir;
    void _usuarioDetectadoCallback();
    void _usuarioNoDetectadoCallback();
public:
    SensorDePresencia(PinName pin, PinMode mode);
    bool HayUsuario();
    void ActualizarEstado();
    EstadoPresencia Estado();
};

#endif