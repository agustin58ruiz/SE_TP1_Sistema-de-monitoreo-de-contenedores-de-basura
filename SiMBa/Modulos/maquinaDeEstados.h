#ifndef MAQUINA_DE_ESTADOS_H
#define MAQUINA_DE_ESTADOS_H

#define MAX_NOMBRE_ESTADO_CHARS 128

#include <functional>
#include "serialPc.h"

typedef std::function<bool()> CondicionFunc_t;
typedef std::function<void()> AccionFunc_t;

//typedef bool (* CondicionFunc_t)(void);
//typedef void (* AccionFunc_t)(void); //int, void*

class Estado;
class MaquinaDeEstados;

typedef std::function<Estado*()> ActualizacionFunc_t;

class Transicion {
public:

    bool virtual Evaluar(MaquinaDeEstados *);
    Transicion virtual &EstablecerCondicion(CondicionFunc_t);
    Transicion virtual &EstablecerAccion(AccionFunc_t);

    void virtual SkipLog(bool);
    bool virtual SkipLog();
};

class TransicionSimple : public Transicion {
private: 
    bool _skipLog;
public:
    CondicionFunc_t Condicion;
    AccionFunc_t Accion;
    Estado *EstadoSiguiente;

    TransicionSimple(CondicionFunc_t, AccionFunc_t, Estado *);
    TransicionSimple(Estado *);

    bool Evaluar(MaquinaDeEstados *);
    TransicionSimple &EstablecerCondicion(CondicionFunc_t);
    TransicionSimple &EstablecerAccion(AccionFunc_t);

    void SkipLog(bool);
    bool SkipLog();
};

class Actualizacion : public Transicion {
private: 
    bool _skipLog;
public:
    AccionFunc_t Accion;
    
    Actualizacion();
    bool Evaluar(MaquinaDeEstados *);

    Actualizacion &EstablecerAccion(AccionFunc_t);
    Actualizacion &EstablecerCondicion(CondicionFunc_t);
    void SkipLog(bool);
    bool SkipLog();
};



typedef Transicion **TransicionesPtr_t;

class Estado {
public:
    const char * Nombre;
    MaquinaDeEstados * MaquinaInterna;
    TransicionesPtr_t Transiciones;
    void **_extraVars;
    int _extraVarsLength;
    
    ActualizacionFunc_t Actualizacion;

    Estado(const char *, TransicionesPtr_t );
    Estado(const char *);

    void Evaluar(MaquinaDeEstados *);
    Estado &EstablecerTransiciones(TransicionesPtr_t);
    void ObtenerInformacion();
    void AsignarMaquinaInterna(MaquinaDeEstados *);

    void EstablecerActualizacion(ActualizacionFunc_t);
    
    
    void BorrarVariables( );
    void *ObtenerVariable(int i);
};

class MaquinaDeEstados {
private:
    Estado * EstadoInicial;
    bool _logs;
    SerialPc* _serial; 

public:
    Estado * EstadoActual;

    MaquinaDeEstados(Estado *);

    void Evaluar(void);
    void ActualizarEstado(Estado *);

    void ObtenerInformacion();
    void Logs(bool);
    bool Logs();
    void EscribirLog(const char* logMensaje);
    void EscribirLog(char c);
    void Reiniciar();
};

#endif