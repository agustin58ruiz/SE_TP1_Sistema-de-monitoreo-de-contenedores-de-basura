#include "arm_book_lib.h"
#include "mbed.h"

#include "sensorDeNivel.h"
#include "sensorDeGas.h"
#include "sensorDeTemperatura.h"
#include "sensorDeProximidad.h"

#include "actuadorTapa.h"
#include "actuadorAlarma.h"
#include "display.h"

#include "maquinaDeEstados.h"
#include "simba.h"

// Definicion de sensores usados por el programa

static ActuadorTapa tapa;
static SensorDeNivel capacidad;
static SensorDeGas sensorDeGas;
static SensorDeTemperatura sensorDeTemperatura(33);
static Display display;
static ActuadorAlarma alarma;
static SensorDeProximidad sensorPir;



Simba::Simba(){

    maquina = nullptr;

}

void Simba::IniciarMaquinaDeEstados(){

    // ------------- DEFINICION DE ESTADOS --------------
    Estado* TapaTrabada = new Estado("TAPA_TRABADA");
    Estado* TapaDestrabada = new Estado("TAPA_DESTRABADA");
    Estado* Inicio = new Estado("INICIO");
    Estado* PresenciaDeGas = new Estado("PRESENCIA_DE_GAS");
    

    // ------------- DEFINICION DE SUBESTADOS --------------
    
    Estado* PirInicio = new Estado("PIR_INICIO");
    Estado* PirPersonaDetectada = new Estado("PIR_PERSONA_DETECTADA");
    Estado* PirPersonaNoDetectada = new Estado("PIR_PERSONA_NO_DETECTADA");
  
    // ---------- CREACION MAQUINA DE ESTADOS -----------

    MaquinaDeEstados* maquina = new MaquinaDeEstados(Inicio);
    // ------ CREACION MAQUINAS DE ESTADOS INTERNA -------

    MaquinaDeEstados* pirMaquina = new MaquinaDeEstados(PirInicio);



    // ---------------- TRANSICIONES---------------------

    Transicion* InicioActualizaciones      = new Transicion( nullptr ); // Esto se va a cambiar a su clase Actualizacion
    Transicion* Inicio2TapaDestrabada      = new Transicion( TapaDestrabada );
    Transicion* Inicio2PresenciaDeGas      = new Transicion( PresenciaDeGas );

    Transicion* TapaTrabadaActualizaciones = new Transicion( nullptr ); // Esto se va a cambiar a su clase Actualizacion
    Transicion* TapaTrabada2TapaDestrabada = new Transicion( TapaDestrabada );
    Transicion* TapaTrabada2PresenciaDeGas = new Transicion( PresenciaDeGas );

    Transicion* TapaDestrabadaActualizaciones = new Transicion( nullptr ); // Esto se va a cambiar a su clase Actualizacion
    Transicion* TapaDestrabada2TapaTrabada = new Transicion( TapaTrabada );
    Transicion* TapaDestrabada2PresenciaDeGas = new Transicion( PresenciaDeGas );

    Transicion* PresenciaDeGasActualizaciones = new Transicion ( nullptr );
    Transicion* PresenciaDeGas2TapaDestrabada = new Transicion( TapaDestrabada );
    Transicion* PresenciaDeGas2TapaTrabada = new Transicion( TapaTrabada );

    // ----------- TRANSICIONES DE SUBESTADOS -----------

    // Sensor PIR
    Transicion* PirInicio2PirPersonaDetectada = new Transicion( PirPersonaDetectada );
    Transicion* PirInicio2PirPersonaNoDetectada = new Transicion( PirPersonaNoDetectada );
    Transicion* PirPersonaDetectada2PirPersonaNoDetectada =  new Transicion( PirPersonaNoDetectada );
    Transicion* PirPersonaNoDetectada2PirPersonaDetectada = new Transicion( PirPersonaDetectada );
    

    // TRANSICIONES INICIO **********************************************************
    TransicionesPtr_t transicionesInicio = new Transicion*[]{InicioActualizaciones ,Inicio2PresenciaDeGas, Inicio2TapaDestrabada,nullptr};
    
    // Inicio Actualizacion
    InicioActualizaciones->EstablecerCondicion([](){
        // Esta actualizacion ocurre solo una vez, por eso esta el display aca. 
        // Mas adelante se puede incluir un atriburto que contenga una accion que ocurra una sola vez. 
       
        sensorDeGas.ActualizarEstado();
        sensorDeTemperatura.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();
        
        display.IniciarDisplay();
        display.CharPositionWrite(0,0);
        display.StringWrite("SiMBA!");
        display.CharPositionWrite(0,1);
        display.StringWrite("Iniciando...");
        display.EstablecerCountdown((std::chrono::microseconds)3000);
        display.ActivarCountdownBacklight();
        return false; // La actualizacion siempre devuelve false
    }).EstablecerAccion([](){

        // La actualizacion nunca realiza una accion. 
    });


    // Inicio -> PresenciaDeGas
    Inicio2PresenciaDeGas->EstablecerCondicion([](){
        
        sensorDeGas.ActualizarEstado();

        return sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO;

    }).EstablecerAccion([](){
        tapa.TrabarTapa();
        alarma.Activar();
        display.Flush();
    });

    // Inicio -> TapaDestrabada
    Inicio2TapaDestrabada->EstablecerCondicion([](){
        return true;
    }).EstablecerAccion([](){
        display.Flush();
    });

    // TRANSICIONES TAPA_DESTRABADA ***********************************************
    TransicionesPtr_t transicionesTapaDestrabada = new Transicion*[]{TapaDestrabadaActualizaciones, TapaDestrabada2TapaTrabada, TapaDestrabada2PresenciaDeGas, nullptr};
    TapaDestrabadaActualizaciones->EstablecerCondicion([](){
       
        sensorDeGas.ActualizarEstado();
        sensorDeTemperatura.ActualizarEstado();
        capacidad.ActualizarEstado();
        tapa.ActualizarEstado();
        sensorPir.ActualizarEstado();

        display.CharPositionWrite(0,0);
        display.StringWrite("Estado Normal");
        display.CharPositionWrite(0,1);
        display.StringWrite("Temp: ");
        display.CharPositionWrite(6,1);
        char temperatura[17]; 
        sensorDeTemperatura.ObtenerTemperatura(temperatura);
        display.StringWrite(temperatura);
        return false;
    }).EstablecerAccion([](){});

    // TapaDestrabada -> TapaTrabada
    TapaDestrabada2TapaTrabada->EstablecerCondicion([](){
        
        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO && sensorDeGas.Estado() == EstadoSensorDeGas::NORMAL;

    }).EstablecerAccion([](){

        display.Flush();
        tapa.TrabarTapa();
        
    });

    TapaDestrabada2PresenciaDeGas->EstablecerCondicion([](){
        return sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO;
    }).EstablecerAccion([](){
        display.Flush();
        tapa.TrabarTapa();
    });

    // TRANSICIONES TAPA TRABADA **********************************************
    

    TransicionesPtr_t transicionesTapaTrabada = new Transicion*[]{TapaTrabadaActualizaciones,TapaTrabada2TapaDestrabada, TapaTrabada2PresenciaDeGas,nullptr};

    // TapaTrabada Actualizaciones
    TapaTrabadaActualizaciones->EstablecerCondicion([](){
        sensorDeGas.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();
        sensorDeTemperatura.ActualizarEstado();
        sensorPir.ActualizarEstado();

        display.CharPositionWrite(0,0);
        display.StringWrite("Estado Bloqueado");
        display.CharPositionWrite(0,1);
        display.StringWrite("LLENO!");

        return false;
    }).EstablecerAccion([](){});

    // TapaTrabada -> TapaDestrabada
    TapaTrabada2TapaDestrabada->EstablecerCondicion([](){


       

        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_CON_ESPACIO && sensorDeGas.Estado() != EstadoSensorDeGas::SATURADO;

    }).EstablecerAccion([](){
        tapa.DestrabarTapa();
        display.Flush();
    });
    // TapaTrabada -> PresenciaDeGas

    TapaTrabada2PresenciaDeGas->EstablecerCondicion([](){
         return sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO;

    }).EstablecerAccion([](){

        display.Flush();
    });

    // TRANSICIONES PRESENCIA DE GAS *******************************************
    
    TransicionesPtr_t transicionesPresenciaDeGas = new Transicion*[]{PresenciaDeGasActualizaciones, PresenciaDeGas2TapaDestrabada, PresenciaDeGas2TapaTrabada, nullptr};
    // PresenciaDeGas Actualizaciones

    PresenciaDeGasActualizaciones->EstablecerCondicion([](){
        sensorDeGas.ActualizarEstado();
        sensorDeTemperatura.ActualizarEstado();
        tapa.ActualizarEstado();
        
        display.CharPositionWrite(0,0);
        display.StringWrite("Peligro!");
        display.CharPositionWrite(0,1);
        display.StringWrite("GAS DETECTADO!");
        alarma.Activar();
        return false;
    }).EstablecerAccion([](){});
    
    // PresenciaDeGas -> TapaDestrabada
    PresenciaDeGas2TapaDestrabada->EstablecerCondicion([](){

        return sensorDeGas.Estado() != EstadoSensorDeGas::SATURADO && capacidad.Estado() == EstadoContenedor::CONTENEDOR_CON_ESPACIO;

    }).EstablecerAccion([](){
        alarma.Desactivar();
        tapa.DestrabarTapa();
        display.Flush();
    });

    // PresenciaDeGas -> TapaTrabada
    PresenciaDeGas2TapaTrabada->EstablecerCondicion([](){
        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO && sensorDeGas.Estado() != EstadoSensorDeGas::SATURADO;
    }).EstablecerAccion([](){
        alarma.Desactivar();
        display.Flush();

    });
    // ------------- TRANSICIONES SUBESTADOS ------------------
    
    // TRANSICIONES PIR INICIO
    TransicionesPtr_t transicionesPirInicio = new Transicion*[]{PirInicio2PirPersonaDetectada, PirInicio2PirPersonaNoDetectada, nullptr};
    // PirInicio -> PersonaDetectada
    PirInicio2PirPersonaDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        //display.Backlight();
        display.ActivarCountdownBacklight();
    });

    // PirInicio -> PersonaNoDetectada
    PirInicio2PirPersonaNoDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_NO_DETECTADO;
    }).EstablecerAccion([](){
        //display.NoBacklight();
    });
    

    // TRANSICIONES PIR PERSONA DETECTADA
    TransicionesPtr_t transicionesPirPersonaDetectada = new Transicion*[]{PirPersonaDetectada2PirPersonaNoDetectada, nullptr};
    // PirPersonaDetectada -> PirPersonaNoDetectada
    PirPersonaDetectada2PirPersonaNoDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_NO_DETECTADO;
    }).EstablecerAccion([](){
        //display.NoBacklight();
    });

    // TRANSICIONES PIR PERSONA NO DETECTADA
    TransicionesPtr_t transicionesPirPersonaNoDetectada = new Transicion*[]{ PirPersonaNoDetectada2PirPersonaDetectada, nullptr};
    // PirPersonaNoDetectada -> PirPersonaDetectada
    PirPersonaNoDetectada2PirPersonaDetectada->EstablecerCondicion([](){


        return sensorPir.Estado() == EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        //display.Backlight();
        display.ActivarCountdownBacklight();
    });



    // ------- Establecer Tranciciones ----------
    Inicio->EstablecerTransiciones( transicionesInicio );
    TapaTrabada->EstablecerTransiciones( transicionesTapaTrabada );
    TapaDestrabada->EstablecerTransiciones( transicionesTapaDestrabada );
    PresenciaDeGas->EstablecerTransiciones( transicionesPresenciaDeGas );

    // ------ Establecer Transiciones de subestados ------
    
    // Subestados: TapaDestrabada
    PirInicio->EstablecerTransiciones( transicionesPirInicio );
    PirPersonaDetectada->EstablecerTransiciones( transicionesPirPersonaDetectada );
    PirPersonaNoDetectada->EstablecerTransiciones( transicionesPirPersonaNoDetectada );

    // Asignar maquina interna

    TapaDestrabada->AsignarMaquinaInterna( pirMaquina );
    TapaTrabada->AsignarMaquinaInterna( pirMaquina );

    this->maquina = maquina; 

};

// METODOS DE LA CLASE SIMBA. 
void Simba::Evaluar(){

    maquina->Evaluar();

}

void Simba::ObtenerInformacion(){
    maquina->ObtenerInformacion();
}

