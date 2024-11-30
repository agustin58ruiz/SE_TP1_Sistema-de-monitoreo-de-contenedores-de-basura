#include "motor.h"
#include "arm_book_lib.h"

#define MAX_INDICE 7
#define INDEX_MOD 8 //This is MAX_INDEX + 1
#define MINUTO_A_SEG 60


static int steps[MAX_INDICE+1] = {
    0b1000,
    0b1100,
    0b0100,
    0b0110,
    0b0010,
    0b0011,
    0b0001,
    0b1001
};

Motor::Motor(PinName Bit0, PinName Bit1, PinName Bit2,PinName Bit3) {
    _control = new BusOut(Bit0,Bit1,Bit2,Bit3);
    _indice = 0;
    *_control = 0b0000;
    _tiempoEntreBobina = 100000;
    _cantidadDePasosRestantes = 0;
}

void Motor::_avanzarUnPaso(){
    *_control = steps[_indice++];
    _cantidadDePasosRestantes--;
    if ( _indice > MAX_INDICE ) _indice=0;
}

void Motor::_retrocederUnPaso(){
    *_control = steps[_indice--];
    _cantidadDePasosRestantes++;
    if ( _indice < 0 ) _indice=MAX_INDICE;
}

void Motor::EstablecerPPMPorPaso( unsigned int rpm ){
    _tiempoEntreBobina = ((1.0/(rpm*MAX_INDICE))*MINUTO_A_SEG);
}

void Motor::_pasosCallbackAvanzar() {
    if ( _cantidadDePasosRestantes != 0 ) {
        _avanzarUnPaso();
    } else {
        *_control = 0b0000;
        _tickerMotor->detach();
    }
}

void Motor::_pasosCallbackRetroceder(){
    if ( _cantidadDePasosRestantes != 0 ) {
        _retrocederUnPaso();
    } else {
        *_control = 0b0000;
        _tickerMotor->detach();
    }
}

void Motor::Pasos( int cantidadDePasos ) {
    _cantidadDePasosRestantes = cantidadDePasos;
    _cantidadDePasos = cantidadDePasos;

    if ( _tickerMotor == nullptr ) {
        _tickerMotor = new Ticker();
    }
    if ( _cantidadDePasosRestantes > 0 ) {
        _indice = 0;
    } else {
        _indice = MAX_INDICE;
    }
}

int Motor::PasosRestantes(){
    return _cantidadDePasosRestantes;
}

void Motor::Pausar() {
    _tickerMotor->detach();
    *_control = 0b0000;
}

void Motor::Empezar(){
    if ( _cantidadDePasosRestantes > 0 ) {
        _tickerMotor->attach(callback(this,&Motor::_pasosCallbackAvanzar), _tiempoEntreBobina);
    } else {
        _tickerMotor->attach(callback(this,&Motor::_pasosCallbackRetroceder), _tiempoEntreBobina);
    }
}

void Motor::Parar(){
    _tickerMotor->detach();
    _cantidadDePasosRestantes = _cantidadDePasos;
     if ( _cantidadDePasosRestantes > 0 ) {
        _indice = 0;
    }else{
        _indice = MAX_INDICE;
    }
    *_control = 0b0000;
}

