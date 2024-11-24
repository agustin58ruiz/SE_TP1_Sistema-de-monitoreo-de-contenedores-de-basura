#ifndef SIMBA_H
#define SIMBA_H

#include "maquinaDeEstados.h"

class Simba {
private:
    MaquinaDeEstados * maquina;
public:
    Simba();
    void IniciarMaquinaDeEstados(void);
    void ObtenerInformacion(void);
    void Evaluar(void);
};

#endif