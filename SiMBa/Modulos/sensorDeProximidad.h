
#ifndef SENSOR_DE_PROXIMIDAD_H
#define SENSOR_DE_PROXIMIDAD_H

#include "mbed.h"


enum class EstadoPresencia{USUARIO_DETECTADO, USUARIO_NO_DETECTADO};

class SensorDeProximidad{
    
private:
    DigitalIn Pir;
    EstadoPresencia EstadoPresenciaUsuario; 
public:

    SensorDeProximidad(PinName pin, PinMode mode);
    bool HayUsuario();
    void ActualizarEstado();
    EstadoPresencia Estado();

};
#endif