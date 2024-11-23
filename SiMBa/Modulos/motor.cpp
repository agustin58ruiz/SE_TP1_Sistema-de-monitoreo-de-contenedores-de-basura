#include "motor.h"
#include "arm_book_lib.h"

#define MAX_INDICE 7
#define INDEX_MOD 8 //This is MAX_INDEX + 1
#define MINUTO_A_US 60000000


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

Motor::Motor(PinName Bit0, PinName Bit1, PinName Bit2,PinName Bit3)
{
    
    _control = new BusOut(Bit0,Bit1,Bit2,Bit3);

    _indice = 0;

    *_control = 0b0000;

    _tiempoEntreBobina = 100000;

    _revertir = false;

    _cantidadDePasosRestantes = 0;

    _funcionando = false;
   
}



void Motor::_avanzarUnPaso(){
    
    *_control = steps[_indice++];
    _cantidadDePasosRestantes--;
    if(_indice > MAX_INDICE) _indice=0;
    
    
}

void Motor::_retrocederUnPaso(){

    *_control = steps[_indice--];
    _cantidadDePasosRestantes++;
    if(_indice < 0) _indice=MAX_INDICE;

}

void Motor::EstablecerRPMPorPaso(unsigned int rpm){

    _tiempoEntreBobina = (unsigned int)((1.0/(rpm*MAX_INDICE))*MINUTO_A_US);

}

void Motor::_pasosCallbackAvanzar(){

    if(_cantidadDePasosRestantes != 0){
        
        _avanzarUnPaso();
        
    } else {

        *_control = 0b0000;
        _tickerMotor->detach();

    }

}
void Motor::_pasosCallbackRetroceder(){

    if(_cantidadDePasosRestantes != 0){
        
        _retrocederUnPaso();
        
    } else {
        
        *_control = 0b0000;
        _tickerMotor->detach();

    }

}

void Motor::Pasos( int cantidadDePasos ) {

    _cantidadDePasosRestantes = cantidadDePasos;
    _pasos = cantidadDePasos;

    if ( _tickerMotor != nullptr ){
        _tickerMotor = new Ticker();
    }
    if(_cantidadDePasosRestantes > 0){
        _indice = 0;
        _tickerMotor->attach(callback(this,&Motor::_pasosCallbackAvanzar), _tiempoEntreBobina*1e-6);
    }else{
        _indice = MAX_INDICE;
        _tickerMotor->attach(callback(this,&Motor::_pasosCallbackRetroceder), _tiempoEntreBobina*1e-6);
    }
}

/*
void Motor::Pasos(int cantidadDePasos){

    _cantidadDePasosRestantes = cantidadDePasos;
    _cantidadDePasos = cantidadDePasos;
    if (_cantidadDePasosRestantes != 0) {
      if (_cantidadDePasosRestantes > 0) {
        while (_cantidadDePasosRestantes != 0) {
          if (_revertir == false) {
            _funcionando = true;
            _avanzarUnPaso();
            _cantidadDePasosRestantes--;
          } else {
            _retrocederUnPaso();
            if (_cantidadDePasosRestantes++ == cantidadDePasos)
              break;
          }
        }
      } else {
        while (_cantidadDePasosRestantes < 0) {
          if (_revertir == false) {
            
            _funcionando = true;
            _retrocederUnPaso();
            _cantidadDePasosRestantes++;
          } else {
              _avanzarUnPaso();
              if (_cantidadDePasosRestantes-- == cantidadDePasos)
                break;
          }
        }
      }
      _revertir = false;

    }
    _funcionando = false;
}

int Motor::PasosRestantes(){

    return _cantidadDePasosRestantes;
}
*/

void Motor::_revertirCallback(){
            
    if(_funcionando == true)
        _revertir = true;
}

void Motor::DesactivarInterrupcionRevertir(){
    if(_revertirInt != nullptr){
        _revertirInt->disable_irq();
    }
}


void Motor::ActivarInterrupcionRevertir(){
    if(_revertirInt != nullptr){
        _revertirInt->enable_irq();
    }
}
void Motor::EstablecerInterrupcionRevertir(PinName pin, PinMode mode){
// Este metodo busca deshacer los pasos si se detecta un evento. 
    if (_revertirInt == nullptr){
        _revertirInt = new InterruptIn(pin);
        _revertirInt->mode(mode);
        
        _revertirInt->fall(callback(this, &Motor::_revertirCallback));    
        
        

    }

}

