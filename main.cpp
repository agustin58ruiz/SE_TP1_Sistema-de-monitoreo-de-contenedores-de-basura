#include "simba.h"
#include "wi-fi.h"
#include "app.h"


#include "arm_book_lib.h"

int main(){

    Simba simba; 
    simba.IniciarMaquinaDeEstados();
    simba.Logs(true);

    App app(&simba);
    

    Wifi wifi(&app);
    wifi.Logs(false);
    wifi.ComInit();


    
    while( true ) {
        
        //printf("WIFI START EVALUATION\r\n");
        wifi.ComUpdate();
        //printf("WIFI END EVALUATION\r\n");
        //delay(10);
        //wifi.wifi->write( "AT\r\n", strlen("AT\r\n") );
        //printf("SIMBA START EVALUATION\r\n");
        //simba.ObtenerInformacion();
    
        simba.Evaluar();
        //printf("SIMBA END EVALUATION\r\n");
    }    
}