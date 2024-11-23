
#ifndef SENSOR_DE_PROXIMIDAD_H
#define SENSOR_DE_PROXIMIDAD_H

#include "mbed.h"


enum class EstadoPresencia{USUARIO_DETECTADO, USUARIO_NO_DETECTADO};

class SensorDeProximidad{
    
private:
    DigitalIn Pir;
    EstadoPresencia EstadoPresenciaUsuario; 
    InterruptIn * _pir;
    void _usuarioDetectadoCallback();
    void _usuarioNoDetectadoCallback();
public:

    SensorDeProximidad(PinName pin, PinMode mode);
    bool HayUsuario();
    void ActualizarEstado();
    EstadoPresencia Estado();

};
#endif