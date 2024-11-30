#include "arm_book_lib.h"
#include "mbed.h"

#include "sensorDeNivel.h"
#include "sensorDeGas.h"
#include "sensorDeTemperatura.h"
#include "sensorDePresencia.h"
#include "temporizador.h"
#include "actuadorTapa.h"
#include "actuadorAlarma.h"
#include "display.h"
#include "motor.h"

#include "maquinaDeEstados.h"
#include "simba.h"

// Definicion de Macros
#define MOTOR_PASOS 1024
#define MOTOR_PPM 5000
#define TIEMPO_ESPERA_SEC 30
#define SIMBA_DESPEDIDA "SiMBa - Adios!"
#define BACKLIGHT_TEMPORIZADOR_SEC 30 

// Definicion de sensores usados por el programa
static ActuadorTapa tapa;
static SensorDeNivel capacidad;
static SensorDeGas sensorDeGas(A0, 0.5, 0.001);
static SensorDeTemperatura sensorDeTemperatura(33);
static Display display;
static ActuadorAlarma alarma;
static SensorDePresencia sensorPir(D3, PullDown);
static Motor motor(D4,D5,D6,D7);
static Temporizador espera(TIEMPO_ESPERA_SEC);

Simba::Simba() {
    maquina = nullptr;
}

void Simba::IniciarMaquinaDeEstados() {

    // ------------- DEFINICION DE ESTADOS --------------
    Estado* TapaTrabada = new Estado( "TAPA_TRABADA" );
    Estado* TapaDestrabada = new Estado( "TAPA_DESTRABADA" );
    Estado* Inicio = new Estado( "INICIO" );
    Estado* PresenciaDeGas = new Estado( "PRESENCIA_DE_GAS" );

    Estado* AbriendoTapa = new Estado( "ABRIENDO_TAPA" );
    Estado* TapaAbierta = new Estado( "TAPA_ABIERTA" );
    Estado* TapaCerrada = new Estado( "TAPA_CERRADA" );
    Estado* CerrandoTapa = new Estado( "CERRANDO_TAPA" );

    // ------------- DEFINICION DE SUBESTADOS --------------
    Estado* PirInicio = new Estado( "PIR_INICIO" );
    Estado* PirPersonaDetectada = new Estado( "PIR_PERSONA_DETECTADA" );
    Estado* PirPersonaNoDetectada = new Estado( "PIR_PERSONA_NO_DETECTADA" );
  
    // ---------- CREACION MAQUINA DE ESTADOS -----------
    MaquinaDeEstados* maquina = new MaquinaDeEstados(Inicio);

    // ------ CREACION MAQUINAS DE ESTADOS INTERNA -------
    MaquinaDeEstados* pirMaquina = new MaquinaDeEstados(PirInicio);

    // ---------------- TRANSICIONES---------------------
    Transicion* InicioActualizaciones      = new Transicion( nullptr ); 
    Transicion* Inicio2TapaDestrabada      = new Transicion( TapaDestrabada );
    Transicion* Inicio2PresenciaDeGas      = new Transicion( PresenciaDeGas );

    Transicion* TapaTrabadaActualizaciones = new Transicion( nullptr ); 
    Transicion* TapaTrabada2TapaDestrabada = new Transicion( TapaDestrabada );
    Transicion* TapaTrabada2PresenciaDeGas = new Transicion( PresenciaDeGas );

    Transicion* TapaDestrabadaActualizaciones = new Transicion( nullptr ); 
    Transicion* TapaDestrabada2AbriendoTapa = new Transicion( AbriendoTapa );
    Transicion* TapaDestrabada2TapaTrabada = new Transicion( TapaTrabada );
    Transicion* TapaDestrabada2PresenciaDeGas = new Transicion( PresenciaDeGas );

    Transicion* PresenciaDeGasActualizaciones = new Transicion ( nullptr );
    Transicion* PresenciaDeGas2TapaDestrabada = new Transicion( TapaDestrabada );
    Transicion* PresenciaDeGas2TapaTrabada = new Transicion( TapaTrabada );

    Transicion* AbriendoTapa2TapaAbierta = new Transicion( TapaAbierta );
    Transicion* AbriendoTapa2CerrandoTapa = new Transicion( CerrandoTapa );

    Transicion* TapaAbierta2CerrandoTapa = new Transicion( CerrandoTapa );

    Transicion* Cualquiera2PresenciaDeGas = new Transicion( PresenciaDeGas );
  
    Transicion* CerrandoTapa2AbriendoTapa = new Transicion( AbriendoTapa );
    Transicion* CerrandoTapa2TapaCerrada = new Transicion( TapaCerrada );

    Transicion* TapaCerrada2TapaDestrabada = new Transicion( TapaDestrabada );
    Transicion* TapaCerrada2AbriendoTapa = new Transicion( AbriendoTapa );

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
        // Mas adelante se puede incluir un atributo que contenga una accion que ocurra una sola vez. 
        sensorDeTemperatura.ActualizarEstado();
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();
        
        display.IniciarDisplay();
        display.CharPositionWrite(0,0);
        display.StringWrite( "SiMBA!" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "Iniciando..." );
        display.EstablecerCountdown(BACKLIGHT_TEMPORIZADOR_SEC);
        motor.EstablecerPPMPorPaso(MOTOR_PPM);
        
        return false; // La actualizacion siempre devuelve false
    }).EstablecerAccion([](){});

    // Inicio -> PresenciaDeGas
    Inicio2PresenciaDeGas->EstablecerCondicion([](){
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
        display.ActivarCountdownBacklight();
    });

    // TRANSICIONES TAPA_DESTRABADA ***********************************************
    TransicionesPtr_t transicionesTapaDestrabada = new Transicion*[]{TapaDestrabadaActualizaciones, TapaDestrabada2AbriendoTapa, TapaDestrabada2TapaTrabada, TapaDestrabada2PresenciaDeGas, nullptr};
    TapaDestrabadaActualizaciones->EstablecerCondicion([](){   
        sensorDeTemperatura.ActualizarEstado();
        capacidad.ActualizarEstado();
        tapa.ActualizarEstado();

        display.CharPositionWrite(0,0);
        display.StringWrite( "Estado Normal" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "Temp: " );
        display.CharPositionWrite(6,1);
        char temperatura[17]; 
        sensorDeTemperatura.ObtenerTemperatura(temperatura);
        display.StringWrite(temperatura);
        return false;
    }).EstablecerAccion([](){});
    
    // TapaDestrabada -> AbriendoTapa
    TapaDestrabada2AbriendoTapa->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        display.Flush();
        display.CharPositionWrite(0,0);
        display.StringWrite( "Abriendo Tapa" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "ESPERE!" );
        display.DesactivarCountdownBacklight();
        display.Backlight();
        motor.Pasos(MOTOR_PASOS);
        motor.Empezar();
    });

    // TapaDestrabada -> TapaTrabada
    TapaDestrabada2TapaTrabada->EstablecerCondicion([](){
        return capacidad.Estado() == EstadoContenedor::CONTENEDOR_LLENO && sensorDeGas.Estado() == EstadoSensorDeGas::NORMAL;
    }).EstablecerAccion([](){
        display.Flush();
        tapa.TrabarTapa();  
    });

    // TapaDestrabada -> PresenciaDeGas
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
        
        tapa.ActualizarEstado();
        capacidad.ActualizarEstado();
        sensorDeTemperatura.ActualizarEstado();

        display.CharPositionWrite(0,0);
        display.StringWrite( "Estado Bloqueado" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "LLENO!" );

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
        sensorDeTemperatura.ActualizarEstado();
        tapa.ActualizarEstado();
        
        display.CharPositionWrite(0,0);
        display.StringWrite( "Peligro!" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "GAS DETECTADO!" );
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


    // TRANSICIONES ABRIENDO TAPA    *******************************************
    TransicionesPtr_t transicionesAbriendoTapa = new Transicion*[]{ AbriendoTapa2TapaAbierta, AbriendoTapa2CerrandoTapa, nullptr};
    
    // AbriendoTapa -> TapaAbierta
    AbriendoTapa2TapaAbierta->EstablecerCondicion([](){

        return motor.PasosRestantes() == 0;
    }).EstablecerAccion([](){
        motor.Pasos(0);
        display.Flush();
        display.CharPositionWrite(0, 0);
        display.StringWrite( "Tapa Abierta" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "Arroje Residuos" );
    });

    // AbriendoTapa -> CerrandoTapa
    AbriendoTapa2CerrandoTapa->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_NO_DETECTADO;
    }).EstablecerAccion([](){
        motor.Pausar();
        int pasos = motor.PasosRestantes()-MOTOR_PASOS;
        motor.Pasos(pasos);
        
        display.Flush();
        display.CharPositionWrite(0, 0);
        display.StringWrite( "Cerrando Tapa" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "Gracias!" );
        display.Backlight();
        motor.Empezar(); 
    });


    // TRANSICIONES TAPA ABIERTA     *******************************************
    TransicionesPtr_t transicionesTapaAbierta = new Transicion*[]{ Cualquiera2PresenciaDeGas, TapaAbierta2CerrandoTapa, nullptr};
    // TapaAbierta -> CerrandoTapa
    TapaAbierta2CerrandoTapa->EstablecerCondicion([](){
        //sensorPir.ActualizarEstado();
        return sensorPir.Estado() == EstadoPresencia::USUARIO_NO_DETECTADO;
    }).EstablecerAccion([](){
        display.Flush();
        display.CharPositionWrite(0, 0);
        display.StringWrite( "Cerrando Tapa" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "Gracias!" );
        motor.Pasos(-MOTOR_PASOS);
        espera.Empezar();
        motor.Empezar();
    });

    
    // TRANSICIONES TAPA CERRADA     *******************************************
    TransicionesPtr_t transicionesTapaCerrada = new Transicion*[]{ Cualquiera2PresenciaDeGas ,TapaCerrada2AbriendoTapa, TapaCerrada2TapaDestrabada, nullptr};
    
    // TapaCerrada -> AbriendoTapa
    TapaCerrada2AbriendoTapa->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        display.Flush();
        display.CharPositionWrite(0,0);
        display.StringWrite( "Abriendo Tapa" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "ESPERE!" );
        display.Backlight();
        motor.Pasos(MOTOR_PASOS);
        espera.Parar();
        motor.Empezar();
    });

    // TapaCerrada -> TapaDestraba
    TapaCerrada2TapaDestrabada->EstablecerCondicion([](){
        return espera.Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
        display.ActivarCountdownBacklight();
    });



    // TRANSICIONES CERRANDO TAPA    *******************************************
    TransicionesPtr_t transicionesCerrandoTapa = new Transicion*[]{ CerrandoTapa2TapaCerrada, CerrandoTapa2AbriendoTapa, nullptr};

    // CerrandoTapa -> TapaCerrada
    CerrandoTapa2TapaCerrada->EstablecerCondicion([](){
        return motor.PasosRestantes() == 0;
    }).EstablecerAccion([](){
        display.Flush();
        display.CharPositionWrite(0,0);
        display.StringWrite( "Tapa Cerrada" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "Gracias!" );
        espera.Empezar();
    });
    
    // CerrandoTapa -> AbriendoTapa
    CerrandoTapa2AbriendoTapa->EstablecerCondicion([](){
        return sensorPir.Estado()== EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        motor.Pausar();
        int pasos = motor.PasosRestantes()+MOTOR_PASOS;
        motor.Pasos(pasos);
        
        display.Flush();
        display.CharPositionWrite(0,0);
        display.StringWrite( "Abriendo Tapa" );
        display.CharPositionWrite(0,1);
        display.StringWrite( "ESPERE!" );
        display.Backlight();
        motor.Empezar();  
    });

    // ------------- TRANSICIONES SUBESTADOS ------------------
    
    // TRANSICIONES PIR INICIO
    TransicionesPtr_t transicionesPirInicio = new Transicion*[]{PirInicio2PirPersonaDetectada, PirInicio2PirPersonaNoDetectada, nullptr};
    
    // PirInicio -> PersonaDetectada
    PirInicio2PirPersonaDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        display.Backlight();
    });

    // PirInicio -> PersonaNoDetectada
    PirInicio2PirPersonaNoDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_NO_DETECTADO;
    }).EstablecerAccion([](){
        display.NoBacklight();
    });
    

    // TRANSICIONES PIR PERSONA DETECTADA
    TransicionesPtr_t transicionesPirPersonaDetectada = new Transicion*[]{PirPersonaDetectada2PirPersonaNoDetectada, nullptr};

    // PirPersonaDetectada -> PirPersonaNoDetectada
    PirPersonaDetectada2PirPersonaNoDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_NO_DETECTADO;
    }).EstablecerAccion([](){
        display.ActivarCountdownBacklight();
    });

    // TRANSICIONES PIR PERSONA NO DETECTADA
    TransicionesPtr_t transicionesPirPersonaNoDetectada = new Transicion*[]{ PirPersonaNoDetectada2PirPersonaDetectada, nullptr};
    
    // PirPersonaNoDetectada -> PirPersonaDetectada
    PirPersonaNoDetectada2PirPersonaDetectada->EstablecerCondicion([](){
        return sensorPir.Estado() == EstadoPresencia::USUARIO_DETECTADO;
    }).EstablecerAccion([](){
        display.Backlight();        
    });

    // TRANSICIONES COMODIN

    // Cualquiera -> PresenciaDeGas
    Cualquiera2PresenciaDeGas->EstablecerCondicion([](){
        return sensorDeGas.Estado() == EstadoSensorDeGas::SATURADO;
    }).EstablecerAccion([](){
        display.DesactivarCountdownBacklight();
        display.Flush();
        display.Backlight();
    });

    // ------- Establecer Tranciciones ----------
    Inicio->EstablecerTransiciones( transicionesInicio );
    TapaTrabada->EstablecerTransiciones( transicionesTapaTrabada );
    TapaDestrabada->EstablecerTransiciones( transicionesTapaDestrabada );
    PresenciaDeGas->EstablecerTransiciones( transicionesPresenciaDeGas );
    AbriendoTapa->EstablecerTransiciones( transicionesAbriendoTapa );
    CerrandoTapa->EstablecerTransiciones( transicionesCerrandoTapa );
    TapaAbierta->EstablecerTransiciones( transicionesTapaAbierta );
    TapaCerrada->EstablecerTransiciones( transicionesTapaCerrada );

    // ------ Establecer Transiciones de subestados ------
    
    // Subestados: TapaDestrabada
    PirInicio->EstablecerTransiciones( transicionesPirInicio );
    PirPersonaDetectada->EstablecerTransiciones( transicionesPirPersonaDetectada );
    PirPersonaNoDetectada->EstablecerTransiciones( transicionesPirPersonaNoDetectada );

    // Asignar maquina interna
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

