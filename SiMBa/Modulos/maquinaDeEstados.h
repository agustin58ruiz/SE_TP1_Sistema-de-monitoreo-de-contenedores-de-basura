#ifndef MAQUINA_DE_ESTADOS_H
#define MAQUINA_DE_ESTADOS_H

#define MAX_NOMBRE_ESTADO_CHARS 128

#include <functional>

typedef std::function<bool()> CondicionFunc_t;
typedef std::function<void()> AccionFunc_t;

//typedef bool (* CondicionFunc_t)(void);
//typedef void (* AccionFunc_t)(void); //int, void*

class Estado;
class MaquinaDeEstados;

class Transicion {
public:
    CondicionFunc_t Condicion;
    AccionFunc_t Accion;
    Estado *EstadoSiguiente;

    Transicion(CondicionFunc_t, AccionFunc_t, Estado *);
    Transicion(Estado *);

    bool Evaluar(MaquinaDeEstados *);
    Transicion &EstablecerCondicion(CondicionFunc_t);
    Transicion &EstablecerAccion(AccionFunc_t);
};

typedef Transicion **TransicionesPtr_t;

class Estado {
public:
    const char * Nombre;
    MaquinaDeEstados * MaquinaInterna;
    TransicionesPtr_t Transiciones;

    Estado(const char *, TransicionesPtr_t );
    Estado(const char *);

    void Evaluar(MaquinaDeEstados *);
    void EstablecerTransiciones(TransicionesPtr_t);
    void ObtenerInformacion();
    void AsignarMaquinaInterna(MaquinaDeEstados *);
};

class MaquinaDeEstados {
private:
    Estado * EstadoInicial;
public:
    Estado * EstadoActual;

    MaquinaDeEstados(Estado *);

    void Evaluar(void);
    void ActualizarEstado(Estado *);
    void ObtenerInformacion();
    void Reiniciar();
};

#endif