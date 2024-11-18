#include "motor.h"
#include "arm_book_lib.h"

#define MAX_INDICE 7
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

    _cantidadDePasos = 0;

    _funcionando = false;
   
}



void Motor::_avanzarUnPaso(){
    for(int i=0; i <= MAX_INDICE; i++){
        *_control = steps[i];
        wait_us(_tiempoEntreBobina);
    }
    *_control = 0b0000;
}

void Motor::_retrocederUnPaso(){
    for(int i = MAX_INDICE; i >= 0; i--){
        *_control = steps[i];
        wait_us(_tiempoEntreBobina);
    }
    *_control = 0b0000;
}

void Motor::EstablecerRPMPorPaso(unsigned int rpm){

    _tiempoEntreBobina = (unsigned int)((1.0/(rpm*MAX_INDICE))*MINUTO_A_US);

}

void Motor::_pasosCallback(){

    if(_cantidadDePasos != 0){
        if (_cantidadDePasos > 0) {
        
          if (_revertir == false) {
            _funcionando = true;
            _avanzarUnPaso();
            _cantidadDePasos--;
          } else {
            _retrocederUnPaso();
          }
        
      } else {
        
          if (_revertir == false) {
            
            _funcionando = true;
            _retrocederUnPaso();
            _cantidadDePasos++;
          } else {
              _avanzarUnPaso();
          }
        
      }
    } else {
        _tickerMotor->detach();

    }

}

void Motor::PasosNew(int cantidadDePasos){

    _cantidadDePasos = cantidadDePasos;
    _pasos= cantidadDePasos;

    if ( _tickerMotor != nullptr ){
        _tickerMotor = new Ticker();
    }
    _tickerMotor->attach(callback(this,&Motor::_pasosCallback), _tiempoEntreBobina*1e-6);
}

void Motor::Pasos(int cantidadDePasos){

    _cantidadDePasos = cantidadDePasos;

    if (_cantidadDePasos != 0) {
      if (_cantidadDePasos > 0) {
        while (_cantidadDePasos != 0) {
          if (_revertir == false) {
            _funcionando = true;
            _avanzarUnPaso();
            _cantidadDePasos--;
          } else {
            _retrocederUnPaso();
            if (_cantidadDePasos++ == cantidadDePasos)
              break;
          }
        }
      } else {
        while (_cantidadDePasos < 0) {
          if (_revertir == false) {
            
            _funcionando = true;
            _retrocederUnPaso();
            _cantidadDePasos++;
          } else {
              _avanzarUnPaso();
              if (_cantidadDePasos-- == cantidadDePasos)
                break;
          }
        }
      }
      _revertir = false;

    }
    _funcionando = false;
}

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

