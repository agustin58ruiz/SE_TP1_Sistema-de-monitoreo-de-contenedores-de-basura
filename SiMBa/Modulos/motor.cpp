#include "motor.h"
#include "arm_book_lib.h"

#define MAX_INDICE 6


static int steps[] = {
    0b1000,
    0b1100,
    0b0100,
    0b0110,
    0b0010,
    0b0011,
    0b0001
};

Motor::Motor(PinName Bit0, PinName Bit1, PinName Bit2,PinName Bit3)
{
    
    _control = new BusOut(Bit0,Bit1,Bit2,Bit3);

    _indice = 0;

    *_control = steps[_indice];
   
}

void Motor::Avanzar(){
    if (_indice++ < (MAX_INDICE + 1)){
        
        *_control = steps[_indice];

    }else{

        _indice = 0;
        *_control = steps[_indice];

    }
}

void Motor::Retroceder(){
    if(_indice-- >= 0){
    
        *_control = steps[_indice];
    
    }else{
    
        _indice = MAX_INDICE;
        *_control = steps[_indice];
    
    }
}


