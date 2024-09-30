#include "maquinaDeEstados.h"
#include "mbed.h"

// Transiciones

Transicion::Transicion(CondicionFunc_t condicion, AccionFunc_t accion, Estado * estadoSiguiente): Condicion(condicion), Accion(accion), EstadoSiguiente(estadoSiguiente){
}

Transicion::Transicion( Estado * estadoSiguiente): Condicion(nullptr), Accion(nullptr), EstadoSiguiente(estadoSiguiente){
}

Transicion &Transicion::EstablecerCondicion(CondicionFunc_t condicion){
    Condicion = condicion;
    return *this;
}

Transicion &Transicion::EstablecerAccion(AccionFunc_t accion){
    Accion = accion;
    return *this;
}

bool Transicion::Evaluar(MaquinaDeEstados* maquina){

    if(Condicion() == true){
        Accion();
        maquina->ActualizarEstado(this->EstadoSiguiente);
        return true;
    }else{
        return false;
    }

}

// Estados

Estado::Estado(char *nombre , TransicionesPtr_t transiciones): Transiciones(transiciones) {
    
    Nombre = nombre;
    this->Transiciones = transiciones;

}

Estado::Estado(char * nombre):Nombre(nombre), Transiciones(nullptr){

}

void Estado::EstablecerTransiciones(TransicionesPtr_t transiciones){

    Transiciones = transiciones;

}
void Estado::Evaluar(MaquinaDeEstados * maquina){
    if (Transiciones != nullptr){

        bool cambio = false;
        for(TransicionesPtr_t i = Transiciones; *i != nullptr && !cambio ; i++){
            Transicion* transicionPtr = *i;
            cambio = transicionPtr->Evaluar(maquina);
        }
    }
}
void Estado::ObtenerInformacion(){
    printf("El estado actual es: %s", Nombre);
}

// Maquina De estados

MaquinaDeEstados::MaquinaDeEstados(Estado * estadoActual): EstadoActual( estadoActual){

}

void MaquinaDeEstados::Evaluar(){

    EstadoActual->Evaluar(this);
}

void MaquinaDeEstados::ActualizarEstado( Estado* estadoSiguiente){

    EstadoActual = estadoSiguiente;
}

void MaquinaDeEstados::ObtenerInformacion(){
    if( EstadoActual != nullptr){
        EstadoActual->ObtenerInformacion();
    }
}






