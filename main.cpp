#include "simba.h"
#include "wi-fi.h"

#include "arm_book_lib.h"

int main(){
    Wifi wifi;
    wifi.ComInit();

    Simba simba; 
    simba.IniciarMaquinaDeEstados();
    
    while( true ) {
        wifi.ComUpdate();
        //delay(10);
       //wifi.wifi->write( "AT\r\n", strlen("AT\r\n") );
        //simba.Evaluar();
        //simba.ObtenerInformacion();
    }
}