#include "maquinaDeEstados.h"
#include "mbed.h"
#include "serialPc.h"

static char logMensaje[129] = "";


// Transiciones
TransicionSimple::TransicionSimple(CondicionFunc_t condicion, AccionFunc_t accion, Estado * estadoSiguiente): Condicion(condicion), Accion(accion), EstadoSiguiente(estadoSiguiente), _skipLog(false) {
}

TransicionSimple::TransicionSimple( Estado * estadoSiguiente): Condicion(nullptr), Accion(nullptr), EstadoSiguiente(estadoSiguiente), _skipLog(false) {
}

TransicionSimple &TransicionSimple::EstablecerCondicion(CondicionFunc_t condicion) {
    Condicion = condicion;
    return *this;
}

TransicionSimple &TransicionSimple::EstablecerAccion(AccionFunc_t accion) {
    Accion = accion;
    return *this;
}

bool TransicionSimple::Evaluar(MaquinaDeEstados* maquina) {
    if( Condicion() == true ) {
        
        if ( maquina->Logs() && !SkipLog() ){
            sprintf(logMensaje, "TRANSICION Simple: %s ----> %s\r\n", maquina->EstadoActual->Nombre, this->EstadoSiguiente->Nombre);
            maquina->EscribirLog( logMensaje );
        }
        Accion();
        maquina->ActualizarEstado( this->EstadoSiguiente );
        return true;
    } else {
        return false;
    }
}

void TransicionSimple::SkipLog( bool skip ) { 
    _skipLog = skip; 
}

bool TransicionSimple::SkipLog() { 
    return _skipLog; 
}

Actualizacion::Actualizacion(): Accion(nullptr), _skipLog(false) {}
bool Actualizacion::Evaluar(MaquinaDeEstados* maquina) {
    if ( maquina->Logs() && !SkipLog() ){
        sprintf(logMensaje, "ACTUALIZACION: %s (ACTUALIZADO) ----> %s \r\n", maquina->EstadoActual->Nombre, maquina->EstadoActual->Nombre);
        maquina->EscribirLog( logMensaje );
    }
    Accion();
    return false;
}

Actualizacion& Actualizacion::EstablecerAccion(AccionFunc_t accion) {
    Accion = accion;
    return *this;
}
Actualizacion& Actualizacion::EstablecerCondicion(CondicionFunc_t condicion) { return *this; }


void Actualizacion::SkipLog( bool skip ) { 
    _skipLog = skip; 
}

bool Actualizacion::SkipLog() { 
    return _skipLog; 
}

// Estados
Estado::Estado(const char *nombre , TransicionesPtr_t transiciones): Transiciones(transiciones) {
    Nombre = nombre;
    this->MaquinaInterna = nullptr;
    _extraVars = nullptr;
    Actualizacion = nullptr;
}

Estado::Estado(const char *nombre): Nombre(nombre), Transiciones(nullptr), MaquinaInterna(nullptr), Actualizacion(nullptr){
    _extraVars = nullptr;
}

Estado &Estado::EstablecerTransiciones(TransicionesPtr_t transiciones) {
    Transiciones = transiciones;
    return *this;
}

void Estado::Evaluar(MaquinaDeEstados * maquina) {
    bool cambio = false;

    if ( Actualizacion != nullptr ) {

        Estado * estadoSiguiente = this->Actualizacion();
        if ( estadoSiguiente != nullptr ){
            maquina->ActualizarEstado( estadoSiguiente );
        }
        return;
    }
    if ( Transiciones != nullptr ) {
        for( TransicionesPtr_t i = Transiciones; *i != nullptr && !cambio; i++ ) {
            Transicion* transicionPtr = *i;
            cambio = transicionPtr->Evaluar(maquina);
        }
    }
    if ( MaquinaInterna != nullptr ) {
        if ( !cambio ) {
            MaquinaInterna->Evaluar();
        }else{
            MaquinaInterna->Reiniciar();
        }
    }
}
void Estado::ObtenerInformacion() {
    printf("%s", Nombre);
    if ( MaquinaInterna != nullptr ) {
        printf(" -> ");
        MaquinaInterna->ObtenerInformacion();
    }
    printf("\n");
}

void Estado::AsignarMaquinaInterna(MaquinaDeEstados * maquinaInterna) {  
    this->MaquinaInterna = maquinaInterna;
}

void Estado::EstablecerActualizacion(ActualizacionFunc_t actualizacion) {
    Actualizacion = actualizacion;
}


void * Estado::ObtenerVariable( int i ) {
    if ( _extraVars == nullptr ) return nullptr;
    if ( i < 0 || i >= _extraVarsLength ) return nullptr;

    return _extraVars[i];
}

// Maquina De estados
MaquinaDeEstados::MaquinaDeEstados(Estado * estadoInicial): EstadoInicial(estadoInicial) {
    EstadoActual = estadoInicial;  
    _logs = false; 
    _serial = nullptr;
}

void MaquinaDeEstados::Evaluar() {
    EstadoActual->Evaluar(this);
}

void MaquinaDeEstados::ActualizarEstado( Estado* estadoSiguiente) {
    EstadoActual = estadoSiguiente;
}

void MaquinaDeEstados::ObtenerInformacion() {
    if( EstadoActual != nullptr) {
        EstadoActual->ObtenerInformacion();
    }
}

void MaquinaDeEstados::Logs( bool logs ) {
    if ( _serial != nullptr ) {
        _serial = new SerialPc();
    }
        
    _logs = logs;
}

bool MaquinaDeEstados::Logs() {
    return _logs;
}

void MaquinaDeEstados::EscribirLog( const char * logMensaje ) {
    if ( _serial != nullptr ) {
        _serial->Enviar( logMensaje );
    } else {
        _serial = new SerialPc();
        _serial->Enviar( logMensaje );
    }

}

void MaquinaDeEstados::EscribirLog( char c ) {
    if ( _serial != nullptr ) {
        _serial->Enviar( c );
    } else {
        printf( "%c", c );
    }

}

void MaquinaDeEstados::Reiniciar() {
    EstadoActual = EstadoInicial;
}
