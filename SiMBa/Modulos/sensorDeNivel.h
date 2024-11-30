#ifndef SENSOR_DE_NIVEL_H
#define SENSOR_DE_NIVEL_H

#include "mbed.h"

enum class EstadoContenedor{CONTENEDOR_LLENO, CONTENEDOR_CON_ESPACIO} ;

<<<<<<< HEAD:SiMBa/sensorDeNivel.h
class SensorDeNivel {
    
=======
class SensorDeNivel{
>>>>>>> TP3:SiMBa/Modulos/sensorDeNivel.h
private:
    DigitalIn Desborde;
    EstadoContenedor EstadoDelContenedor; 
public:
    SensorDeNivel(PinName pin, PinMode mode);
    SensorDeNivel();
    bool EstaDesbordado();
    void ActualizarEstado();
    EstadoContenedor Estado();
};

#endif