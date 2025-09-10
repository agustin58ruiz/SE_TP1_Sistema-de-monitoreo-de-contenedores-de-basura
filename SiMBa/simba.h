#ifndef SIMBA_H
#define SIMBA_H

#include "maquinaDeEstados.h"

class Simba {
private:
    MaquinaDeEstados * maquina;
    bool _logs;
    bool _modoManual;
public:
    Simba();
    void IniciarMaquinaDeEstados(void);
    void ObtenerInformacion(void);
    void Evaluar(void);
    void Logs(bool mostrarLogs);

    // Dispositivos
    void ObtenerTemperatura(char * str);
    void ActivarModoManual();
    void DesactivarModoManual();

    void AbrirTapa();
    void CerrarTapa();

    void ApagarAlarma();
    void EncenderAlarma();

};

#endif