#include "simba.h"

int main(){
    Simba simba; 
    simba.IniciarMaquinaDeEstados();
    while( true ) {
        simba.Evaluar();
        simba.ObtenerInformacion();
    }
}