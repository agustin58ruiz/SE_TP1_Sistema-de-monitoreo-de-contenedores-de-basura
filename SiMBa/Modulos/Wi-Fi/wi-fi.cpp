#include "wi-fi.h"

#include "maquinaDeEstados.h"
#include "temporizador.h"
#include "mbed.h"
#include <cstddef>

#define DELAY_5_SECONDS 5
#define DELAY_10_SECONDS 10

#define IP_MAX_LENGTH (15 + 1)

static UnbufferedSerial uartWifi( PE_8, PE_7, 115200 );

static const char responseOk[] = "OK";
static const char responseCwjapOk[] = "+CWJAP:";
static const char responseCwjap1[] = "WIFI CONNECTED";
static const char responseCwjap2[] = "WIFI GOT IP";
static const char responseCifsr[] = "+CIFSR:STAIP,\"";
static const char responseStatus3[] = "STATUS:3";
static const char responseCipstatus[] = "+CIPSTATUS:";
static const char responseSendOk[] = "SEND OK";
static const char responseCipclose[] = "CLOSED";

static int currentConnectionId;
static char wifiComApSsid[AP_SSID_MAX_LENGTH] = "";
static char wifiComApPassword[AP_PASSWORD_MAX_LENGTH] = "";
static char wifiComIpAddress[IP_MAX_LENGTH];

Wifi::Wifi(){
    _maquina = nullptr;
    _delay = new Temporizador();
    wifi = &uartWifi;
}

void Wifi::ComInit(){
    // ------------- DEFINICION DE ESTADOS --------------

    Estado* Init = new Estado( "WIFI_STATE_INIT" );
    Estado* At = new Estado( "WIFI_STATE_SEND_AT" );
    Estado* WaitAt = new Estado( "WIFI_STATE_WAIT_AT" );
    Estado* CwMode = new Estado( "WIFI_STATE_SEND_CWMODE" );
    Estado* WaitCwMode = new Estado( "WIFI_STATE_WAIT_CWMODE" );
    Estado* CwJapIsSet = new Estado( "WIFI_STATE_SEND_CWJAP_IS_SET" );
    Estado* WaitCwJapIsSet = new Estado( "WIFI_STATE_WAIT_CWJAP_IS_SET" );
    Estado* CwJapSet = new Estado( "WIFI_STATE_SEND_CWJAP_SET" );
    Estado* WaitCwJapSet1 = new Estado( "WIFI_STATE_WAIT_CWJAP_SET_1" );
    Estado* WaitCwJapSet2 = new Estado( "WIFI_STATE_WAIT_CWJAP_SET_2" );
    Estado* Cifsr = new Estado( "WIFI_STATE_SEND_CIFSR" );
    Estado* WaitCifsr = new Estado( "WIFI_STATE_WAIT_CIFSR" );
    Estado* LoadIp = new Estado( "WIFI_STATE_LOAD_IP" );
    Estado* CipMux = new Estado( "WIFI_STATE_SEND_CIPMUX" );
    Estado* WaitCipMux = new Estado( "WIFI_STATE_WAIT_CIPMUX" );
    Estado* CipServer = new Estado( "WIFI_STATE_SEND_CIPSERVER" );
    Estado* WaitCipServer = new Estado( "WIFI_STATE_WAIT_CIPSERVER" );
    Estado* CipStatus = new Estado( "WIFI_STATE_SEND_CIPSTATUS" );
    Estado* WaitCipStatusStatus3 = new Estado( "WIFI_STATE_WAIT_CIPSTATUS_STATUS_3" );
    Estado* WaitCipStatus = new Estado( "WIFI_STATE_WAIT_CIPSTATUS" );
    Estado* WaitGetId = new Estado( "WIFI_STATE_WAIT_GET_ID" );
    Estado* WaitCipStatusOk = new Estado( "WIFI_STATE_WAIT_CIPSTATUS_OK" );
    Estado* CipSend = new Estado( "WIFI_STATE_SEND_CIPSEND" );
    Estado* WaitCipSend = new Estado( "WIFI_STATE_WAIT_CIPSEND" );
    Estado* SendHtml = new Estado( "WIFI_STATE_SEND_HTML" );
    Estado* WaitHtml = new Estado( "WIFI_STATE_WAIT_HTML" );
    Estado* CipClose = new Estado( "WIFI_STATE_SEND_CIPCLOSE" );
    Estado* WaitCipClose = new Estado( "WIFI_STATE_WAIT_CIPCLOSE" );
    Estado* Idle = new Estado( "WIFI_STATE_IDLE" );
    Estado* Error = new Estado( "WIFI_STATE_ERROR" );

    // ---------- CREACION MAQUINA DE ESTADOS -----------
    MaquinaDeEstados* maquina = new MaquinaDeEstados(Init);

    // ---------------- TRANSICIONES---------------------

    Transicion* Init2At = new Transicion( At );
    Transicion* At2WaitAt = new Transicion( WaitAt );
    Transicion* WaitAt2CwMode = new Transicion( CwMode );
    Transicion* WaitAt2Idle = new Transicion( Idle );
    Transicion* WaitAt2Error = new Transicion( Error );
    Transicion* CwMode2WaitCwMode = new Transicion( WaitCwMode );
    Transicion* WaitCwMode2CwJapIsSet = new Transicion( CwJapIsSet );
    Transicion* WaitCwMode2Error = new Transicion( Error );
    Transicion* CwJapIsSet2WaitCwJapIsSet = new Transicion( WaitCwJapIsSet );
    Transicion* WaitCwJapIsSet2Cifsr = new Transicion( Cifsr );
    Transicion* WaitCwJapIsSet2CwJapSet = new Transicion( CwJapSet );
    Transicion* CwJapSet2WaitCwJapSet1 = new Transicion( WaitCwJapSet1 );
    Transicion* WaitCwJapSet12WaitCwJapSet2 = new Transicion( WaitCwJapSet2 );
    Transicion* WaitCwJapSet12Error = new Transicion( Error );
    Transicion* WaitCwJapSet22Cifsr = new Transicion( Cifsr );
    Transicion* WaitCwJapSet22Error = new Transicion( Error );
    Transicion* Cifsr2WaitCifsr = new Transicion( WaitCifsr );
    Transicion* WaitCifsr2LoadIp = new Transicion( LoadIp );
    Transicion* WaitCifsr2Error = new Transicion( Error );
    Transicion* LoadIp2CipMux = new Transicion( CipMux );
    Transicion* CipMux2WaitCipMux = new Transicion( WaitCipMux );
    Transicion* WaitCipMux2CipServer = new Transicion( CipServer );
    Transicion* WaitCipMux2Error = new Transicion( Error );
    Transicion* CipServer2WaitCipServer = new Transicion( WaitCipServer );
    Transicion* WaitCipServer2CipStatus = new Transicion( CipStatus );
    Transicion* WaitCipServer2Error = new Transicion( Error );
    Transicion* CipStatus2WaitCipStatusStatus3 = new Transicion( WaitCipStatusStatus3 );
    Transicion* WaitCipStatusStatus32WaitCipStatus = new Transicion( WaitCipStatus );
    Transicion* WaitCipStatusStatus3CipStatus = new Transicion( CipStatus );
    Transicion* WaitCipStatus2WaitGetId = new Transicion( WaitGetId );
    Transicion* WaitCipStatus2CipStatus = new Transicion( CipStatus );
    Transicion* WaitGetId2WaitCipStatusOk = new Transicion( WaitCipStatusOk );
    Transicion* WaitCipStatusOk2CipSend = new Transicion( CipSend );
    Transicion* WaitCipStatusOk2CipStatus = new Transicion( CipStatus );
    Transicion* CipSend2WaitCipSend = new Transicion( CipSend );
    Transicion* WaitCipSend2SendHtml = new Transicion( SendHtml );
    Transicion* WaitCipSend2CipStatus = new Transicion( CipStatus );
    Transicion* SendHtml2WaitHtml = new Transicion( WaitHtml );
    Transicion* WaitHtml2CipClose = new Transicion( CipClose );
    Transicion* WaitHtml2CipSend = new Transicion( CipSend );
    Transicion* CipClose2WaitCipClose = new Transicion( WaitCipClose );
    Transicion* WaitCipClose2CipStatus_1 = new Transicion( CipStatus );
    Transicion* WaitCipClose2CipStatus_2 = new Transicion( CipStatus );
    


    // TRANSICIONES INIT ****************************************
    TransicionesPtr_t transicionesInit = new Transicion*[]{ Init2At, nullptr};
    // Init -> At
    Init2At->EstablecerCondicion([]()-> bool {
        return true;
    }).EstablecerAccion([this]()-> void {
        _delay->Empezar(DELAY_5_SECONDS);
    });


    // TRANSICIONES SEND AT *************************************
    TransicionesPtr_t transicionesAt = new Transicion*[]{ At2WaitAt, nullptr };
    // At -> WaitAt
    At2WaitAt->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _comStringWrite("AT\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES WAIT AT *************************************
    TransicionesPtr_t transicionesWaitAt = new Transicion* []{ WaitAt2CwMode, WaitAt2Error, nullptr };
    // WaitAt -> CwMode
    WaitAt2CwMode->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar(DELAY_5_SECONDS);
    });

    // WaitAt -> Error
    WaitAt2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
        printf("AT command not responded ");
        printf("correctly\r\n");
    });

    // TRANSICIONES SEND CWMODE
    TransicionesPtr_t transicionesCwMode = new Transicion*[]{ CwMode2WaitCwMode, nullptr };
    // CwMode -> WaitCwMode 
    CwMode2WaitCwMode->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _comStringWrite("AT+CWMODE=1\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES WAIT CWMODE
    TransicionesPtr_t transicionesWaitCwMode = new Transicion* []{ WaitCwMode2CwJapIsSet, WaitCwMode2Error, nullptr};
    // WaitCwMode -> CwJapIsSet
    WaitCwMode2CwJapIsSet->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar(DELAY_5_SECONDS);
    });
    // WaitCwMode -> Error
    WaitCwMode2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
            printf("AT+CWMODE=1 command not ");
            printf("responded correctly\r\n");
    });
    
    // TRANSICIONES SEND CWJAP IS SET
    TransicionesPtr_t transicionesCwJapIsSet = new Transicion*[]{ CwJapIsSet2WaitCwJapIsSet, nullptr};
    // CwJapIsSet -> WaitCwJapIsSet
    CwJapIsSet2WaitCwJapIsSet->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
       _comStringWrite("AT+CWJAP?\r\n");
       _comExpectedResponse = responseCwjapOk;
       _delay->Empezar(DELAY_5_SECONDS);
    });
    
    // TRANSICIONES WAIT CWJAP IS SET
    TransicionesPtr_t transicionesWaitCwJapIsSet = new Transicion*[] { WaitCwJapIsSet2Cifsr, WaitCwJapIsSet2CwJapSet, nullptr};
    // WaitCwJapIsSet -> Cifsr
    WaitCwJapIsSet2Cifsr->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _comExpectedResponse = responseOk;
    });
    // WaitCwJapIsSet ->CwJapSet
    WaitCwJapIsSet2CwJapSet->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES SEND CWJAP SET
    TransicionesPtr_t transicionesCwJapSet = new Transicion*[] {CwJapSet2WaitCwJapSet1, nullptr};
    // CwJapSet -> WaitCwJapSet1
    CwJapSet2WaitCwJapSet1->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
            _comStringWrite( "AT+CWJAP=\"" );
            _comStringWrite( wifiComApSsid );
            _comStringWrite( "\",\"" );
            _comStringWrite( wifiComApPassword );
            _comStringWrite( "\"" );
            _comStringWrite( "\r\n" );
            _comExpectedResponse = responseCwjap1;
            _delay->Empezar( DELAY_10_SECONDS );
    });

    // TRANSICIONES WAIT CWJAP SET 1
    TransicionesPtr_t transicionesWaitCwJapSet1 = new Transicion*[] { WaitCwJapSet12WaitCwJapSet2, WaitCwJapSet12Error, nullptr };
    // WaitCwJapSet1 -> WaitCwJapSet2
    WaitCwJapSet12WaitCwJapSet2->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _comExpectedResponse = responseCwjap2;
    });

    // WaitCwJapSet1 -> Error
    WaitCwJapSet12Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        printf("Error in state: ");
        printf("WIFI_STATE_WAIT_CWJAP_SET_1\r\n");
        printf("Check Wi-Fi AP credentials ");
        printf("and restart\r\n");
    });

    // TRANSICIONES WAIT CWJAP SET 2
    TransicionesPtr_t transicionesWaitCwJapSet2 = new Transicion*[] { WaitCwJapSet22Cifsr, WaitCwJapSet22Error, nullptr };
    // WaitCwJapSet2 -> Cifsr
    WaitCwJapSet22Cifsr->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([](){});
    // WaitCwJapSet2 -> Error
    WaitCwJapSet22Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
            printf("Error in state: ");
            printf("WIFI_STATE_WAIT_CWJAP_SET_2\r\n");
            printf("Check Wi-Fi AP credentials ");
            printf("and restart\r\n");
    });

    // TRANSICIONES SEND CIFSR
    TransicionesPtr_t transicionesCifsr = new Transicion*[]{};
    // Cifsr -> WaitCifsr
    Cifsr2WaitCifsr->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
            _comStringWrite( "AT+CIFSR\r\n" );
            _comExpectedResponse = responseCifsr;
            _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES WAIT CIFSR
    TransicionesPtr_t transicionesWaitCifsr = new Transicion* []{};
    // WaitCifsr -> LoadIp
    WaitCifsr2LoadIp->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _ipStringPositionIndex = 0;
    });
    // WaitCifsr -> Error
    WaitCifsr2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
        printf("AT+CIFSR command not responded ");
        printf("correctly\r\n");
    });

    // TRANSICIONES LOAD IP
    TransicionesPtr_t transicionesLoadIp = new Transicion*[]{};

    // LoadIp -> CipMux
    LoadIp2CipMux->EstablecerCondicion([this](){
        if( _comCharRead(&_receivedChar) ) {
            if ( ( _receivedChar != '"' ) &&
            ( _ipStringPositionIndex < IP_MAX_LENGTH ) ) {
                wifiComIpAddress[_ipStringPositionIndex] = _receivedChar;
                _ipStringPositionIndex++;
            } else { 
                return true;
            }
        }
        return false;

    }).EstablecerAccion([this](){
        wifiComIpAddress[_ipStringPositionIndex] = '\0';
        printf("IP address assigned correctly\r\n\r\n");
    });

    // TRANSICIONES SEND CIPMUX
    TransicionesPtr_t transicionesCipMux = new Transicion*[]{};
    //CipMux -> WaitCipMux
    CipMux2WaitCipMux->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _comStringWrite( "AT+CIPMUX=1\r\n" );
        _comExpectedResponse = responseOk;
        _delay
    });





   // ------- Establecer Tranciciones ----------
   Init->EstablecerTransiciones( transicionesInit );
   At->EstablecerTransiciones( transicionesAt );
   WaitAt->EstablecerTransiciones( transicionesWaitAt );

    this->_maquina = maquina;

}

void Wifi::ComUpdate(){
    _maquina->Evaluar();
}

void Wifi::ComRestart(){
    _maquina->Reiniciar();
}

// PRIVATE METHODS

bool Wifi::_comCharRead( char* receivedChar ){
    char receivedCharLocal = '\0';
    if( uartWifi.readable() ) {
        uartWifi.read(&receivedCharLocal,1);
        *receivedChar = receivedCharLocal;

        return true;
    }

    return false;
}

void Wifi::_comStringWrite( const char* str ){
    uartWifi.write( str, strlen(str) );
}

bool Wifi::_isExpectedResponse(){
    static int responseStringPositionIndex = 0;
    char charReceived;
    bool moduleResponse = false;

    if( _comCharRead(&charReceived) ){
        if ( charReceived == _comExpectedResponse[responseStringPositionIndex] ) {
            responseStringPositionIndex++;
            if (_comExpectedResponse[responseStringPositionIndex] == '\0') {
                responseStringPositionIndex = 0;
                moduleResponse = true;
            }
        } else {
            responseStringPositionIndex = 0;
        }
    }
   return moduleResponse;
}