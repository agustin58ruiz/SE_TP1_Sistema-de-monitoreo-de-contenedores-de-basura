#include "wi-fi.h"

#include "maquinaDeEstados.h"
#include "temporizador.h"
#include "buffer.h"
#include "app.h"
#include "miscellaneous.h"
#include "wi-fi-status.h"
#include "uart.h"
#include "at.h"

#include "mbed.h"
#include <cstddef>

#define DELAY_1_SECONDS 1
#define DELAY_5_SECONDS 5
#define DELAY_10_SECONDS 10

#define LINKS_NUMBER 5

#define IP_MAX_LENGTH (15 + 1)
#define HTTP_HEADER_LENGTH STRING_2_SEND_MAX_LENGTH - 1
#define BUFFER_LENGTH 128

//static BufferedSerial uartWifi( PE_8, PE_7, 115200 );

static Uart serialWiFi( PE_8, PE_7, 115200 );

static const char responseOk[] = "OK";
static const char responseCwjapOk[] = "+CWJAP:";
static const char responseCwjap1[] = "WIFI CONNECTED";
static const char responseCwjap2[] = "WIFI GOT IP";
static const char responseCifsr[] = "+CIFSR:STAIP,\"";
static const char responseStatus3[] = "STATUS:3";
static const char responseCipstatus[] = "+CIPSTATUS:";
static const char responseSendOk[] = "SEND OK";
static const char responseCipclose[] = "CLOSED";
static const char responseCiprecvdata[] = "+CIPRECVDATA,";
static char responseWildcard[20] = ""; 

static int currentConnectionId;
static char wifiComApSsid[AP_SSID_MAX_LENGTH] = "RuizAgustin";
static char wifiComApPassword[AP_PASSWORD_MAX_LENGTH] = "1tBk1qkwdW";
static char wifiComIpAddress[IP_MAX_LENGTH];

static const char htmlCode [] =
   "<!DOCTYPE html><html><head><title>SiMBA</title></head><body><p>&nbsp;</p><h1>SiMBa&nbsp;</h1><h2>Estad&iacute;sticas:&nbsp;</h2><ul><li>Temperatura:</li><li>Tapa:</li><li>SensorPir:</li><li>Sensor de gas:</li></ul><h2>Acciones:</h2><ul><li>Abrir tapa</li><li>Cerrar Tapa</li></ul></body></html>";

static char lastResponse[50]="";
static int *indexes = nullptr;

static WiFiStatus* wifiStatus=nullptr;
static int linksLengths[LINKS_NUMBER];

Wifi::Wifi(){
    _maquina = nullptr;
    _delay = new Temporizador();
   // wifi = &uartWifi;
    _logs = false;
    _rx = nullptr;
    _app = new App();
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
    Estado* CipRecvMode = new Estado( "WIFI_STATE_SEND_CIPRECVMODE");
    Estado* WaitCipRecvMode = new Estado( "WIFI_STATE_WAIT_CIPRECVMODE");

    /*REDEFINIENDO CIPSTATUS*/
    Estado* CipStatus = new Estado( "WIFI_STATE_SEND_CISTATUS");
    Estado* ProcessingCipStatus = new Estado("WIFI_STATE_PROCESSING_CIPSTATUS"); 



    //Estado* CipStatus = new Estado( "WIFI_STATE_SEND_CIPSTATUS" );
    Estado* WaitCipStatusStatus3 = new Estado( "WIFI_STATE_WAIT_CIPSTATUS_STATUS_3" );
    Estado* WaitCipStatus = new Estado( "WIFI_STATE_WAIT_CIPSTATUS" );
    Estado* WaitGetId = new Estado( "WIFI_STATE_WAIT_GET_ID" );
    Estado* WaitCipStatusOk = new Estado( "WIFI_STATE_WAIT_CIPSTATUS_OK" );

    Estado* CipRecvLenQuery = new Estado( "WIFI_STATE_CIPRECVDATA_QUERY" );
    Estado* WaitCipRecvLenQuery = new Estado( "WIFI_STATE_WAIT_CIPRECVDATA_QUERY");
    
    Estado* CipRecvData = new Estado( "WIFI_STATE_SEND_CIPRECVDATA" );
    Estado* WaitCipRecvData1 = new Estado( "WIFI_STATE_WAIT_CIPRECVDATA1" );
    Estado* WaitCipRecvData2 = new Estado( "WIFI_STATE_WAIT_CIPRECVDATA2" );

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

    Transicion* Init2At = new TransicionSimple( At );
    Transicion* At2WaitAt = new TransicionSimple( WaitAt );
    Transicion* WaitAt2CwMode = new TransicionSimple( CwMode );
    //Transicion* WaitAt2Idle = new TransicionSimple( Idle );
    Transicion* WaitAt2Error = new TransicionSimple( Error );
    Transicion* CwMode2WaitCwMode = new TransicionSimple( WaitCwMode );
    Transicion* WaitCwMode2CwJapIsSet = new TransicionSimple( CwJapIsSet );
    Transicion* WaitCwMode2Error = new TransicionSimple( Error );
    Transicion* CwJapIsSet2WaitCwJapIsSet = new TransicionSimple( WaitCwJapIsSet );
    Transicion* WaitCwJapIsSet2Cifsr = new TransicionSimple( Cifsr );
    Transicion* WaitCwJapIsSet2CwJapSet = new TransicionSimple( CwJapSet );
    Transicion* CwJapSet2WaitCwJapSet1 = new TransicionSimple( WaitCwJapSet1 );
    Transicion* WaitCwJapSet12WaitCwJapSet2 = new TransicionSimple( WaitCwJapSet2 );
    Transicion* WaitCwJapSet12Error = new TransicionSimple( Error );
    Transicion* WaitCwJapSet22Cifsr = new TransicionSimple( Cifsr );
    Transicion* WaitCwJapSet22Error = new TransicionSimple( Error );
    Transicion* Cifsr2WaitCifsr = new TransicionSimple( WaitCifsr );
    Transicion* WaitCifsr2LoadIp = new TransicionSimple( LoadIp );
    Transicion* WaitCifsr2Error = new TransicionSimple( Error );
    Transicion* LoadIp2CipMux = new TransicionSimple( CipMux );
    Transicion* CipMux2WaitCipMux = new TransicionSimple( WaitCipMux );
    Transicion* WaitCipMux2CipServer = new TransicionSimple( CipServer );
    Transicion* WaitCipMux2Error = new TransicionSimple( Error );
    Transicion* CipServer2WaitCipServer = new TransicionSimple( WaitCipServer );
    Transicion* WaitCipServer2CipRecvMode = new TransicionSimple( CipRecvMode );
    Transicion* WaitCipServer2Error = new TransicionSimple( Error );
    Transicion* CipRecvMode2WaitCipRecvMode = new TransicionSimple( WaitCipRecvMode );
    
    Transicion* WaitCipRecvMode2CwState = new TransicionSimple( CipStatus );
    Transicion* ProcessingCipStatusActualizacion = new Actualizacion();
    Transicion* CipStatus2ProcessingCipStatus = new TransicionSimple( ProcessingCipStatus );

    Transicion* ProcessingCipStatus2CipStatus = new TransicionSimple( CipStatus );


    Transicion* ProcessingCipStatus2Error = new TransicionSimple( CipStatus );

    Transicion* WaitCipRecvMode2CipStatus = new TransicionSimple( CipStatus );
    Transicion* WaitCipRecvMode2Error = new TransicionSimple( Error );
    Transicion* CipStatus2WaitCipStatusStatus3 = new TransicionSimple( WaitCipStatusStatus3 );
    Transicion* WaitCipStatusStatus32WaitCipStatus = new TransicionSimple( WaitCipStatus );
    Transicion* WaitCipStatusStatus32CipStatus = new TransicionSimple( CipStatus );
    Transicion* WaitCipStatus2WaitGetId = new TransicionSimple( WaitGetId );
    Transicion* WaitCipStatus2CipStatus = new TransicionSimple( CipStatus );

    Transicion* WaitGetId2WaitCipStatusOk = new TransicionSimple( WaitCipStatusOk );

    Transicion* WaitCipStatusOk2CipRecvDataQuery = new TransicionSimple( CipRecvLenQuery );
    Transicion* CipRecvLenQuery2WaitCipRecvLenQuery = new TransicionSimple( WaitCipRecvLenQuery );
    Transicion* WaitCipRecvLenQuery2CipRecvData = new TransicionSimple( CipRecvData );
    Transicion* WaitCipRecvLenQuery2Error = new TransicionSimple( Error );
    
    Transicion* WaitCipStatusOk2CipRecvData = new TransicionSimple( CipRecvData );
    Transicion* CipRecvData2WaitCipRecvData1 = new TransicionSimple( WaitCipRecvData1 );
    Transicion* WaitCipRecvData12WaitCipRecvData2 = new TransicionSimple( WaitCipRecvData2 );
    Transicion* WaitCipRecvData12Error = new TransicionSimple( Error );
    Transicion* WaitCipRecvData22CipSend = new TransicionSimple( CipSend );
    
    Transicion* CipSend2WaitCipSend = new TransicionSimple( WaitCipSend );
    Transicion* WaitCipSend2SendHtml = new TransicionSimple( SendHtml );
    Transicion* WaitCipSend2CipStatus = new TransicionSimple( CipStatus );
    Transicion* SendHtml2WaitHtml = new TransicionSimple( WaitHtml );
    Transicion* WaitHtml2CipClose = new TransicionSimple( CipClose );
    Transicion* WaitHtml2CipSend = new TransicionSimple( CipSend );
    Transicion* CipClose2WaitCipClose = new TransicionSimple( WaitCipClose );
    Transicion* WaitCipClose2CipStatus_1 = new TransicionSimple( CipStatus );
    Transicion* WaitCipClose2CipStatus_2 = new TransicionSimple( CipStatus );
    


    // TRANSICIONES INIT ****************************************
    Init->EstablecerTransiciones( new Transicion*[]{ Init2At, nullptr} );
    // Init -> At
    Init2At->EstablecerCondicion([this](){        
        return true;
    }).EstablecerAccion([this](){
        _delay->Empezar(DELAY_5_SECONDS);
    });


    // TRANSICIONES SEND AT *************************************
    At->EstablecerTransiciones( new Transicion*[]{ At2WaitAt, nullptr } );
    // At -> WaitAt
    At2WaitAt->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString("AT\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES WAIT AT *************************************
    WaitAt->EstablecerTransiciones( new Transicion* []{ WaitAt2CwMode, WaitAt2Error, nullptr } );
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
    CwMode->EstablecerTransiciones( new Transicion*[]{ CwMode2WaitCwMode, nullptr } );
    // CwMode -> WaitCwMode 
    CwMode2WaitCwMode->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString("AT+CWMODE=1\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES WAIT CWMODE
    WaitCwMode->EstablecerTransiciones( new Transicion* []{ WaitCwMode2CwJapIsSet, WaitCwMode2Error, nullptr} );
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
    CwJapIsSet->EstablecerTransiciones( new Transicion*[]{ CwJapIsSet2WaitCwJapIsSet, nullptr} );
    // CwJapIsSet -> WaitCwJapIsSet
    CwJapIsSet2WaitCwJapIsSet->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
       serialWiFi.WriteString("AT+CWJAP?\r\n");
       _comExpectedResponse = responseCwjapOk;
       _delay->Empezar(DELAY_5_SECONDS);
    });
    
    // TRANSICIONES WAIT CWJAP IS SET
    WaitCwJapIsSet->EstablecerTransiciones( new Transicion*[] { WaitCwJapIsSet2Cifsr, WaitCwJapIsSet2CwJapSet, nullptr} );
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
    CwJapSet->EstablecerTransiciones( new Transicion*[] {CwJapSet2WaitCwJapSet1, nullptr} );
    // CwJapSet -> WaitCwJapSet1
    CwJapSet2WaitCwJapSet1->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
            serialWiFi.WriteString( "AT+CWJAP=\"" );
            serialWiFi.WriteString( wifiComApSsid );
            serialWiFi.WriteString( "\",\"" );
            serialWiFi.WriteString( wifiComApPassword );
            serialWiFi.WriteString( "\"" );
            serialWiFi.WriteString( "\r\n" );
            _comExpectedResponse = responseCwjap1;
            _delay->Empezar( DELAY_10_SECONDS );
    });

    // TRANSICIONES WAIT CWJAP SET 1
    WaitCwJapSet1->EstablecerTransiciones( new Transicion*[] { WaitCwJapSet12WaitCwJapSet2, WaitCwJapSet12Error, nullptr } );
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
    WaitCwJapSet2->EstablecerTransiciones( new Transicion*[] { WaitCwJapSet22Cifsr, WaitCwJapSet22Error, nullptr } );
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
    Cifsr->EstablecerTransiciones( new Transicion*[]{Cifsr2WaitCifsr, nullptr} );
    // Cifsr -> WaitCifsr
    Cifsr2WaitCifsr->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
            serialWiFi.WriteString( "AT+CIFSR\r\n" );
            _comExpectedResponse = responseCifsr;
            _delay->Empezar(DELAY_5_SECONDS);
    });

    // TRANSICIONES WAIT CIFSR
    WaitCifsr->EstablecerTransiciones( new Transicion* []{WaitCifsr2LoadIp, WaitCifsr2Error, nullptr} );
    // WaitCifsr -> LoadIp
    WaitCifsr2LoadIp->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _strPositionIndex = 0;
    });
    // WaitCifsr -> Error
    WaitCifsr2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
        printf("AT+CIFSR command not responded ");
        printf("correctly\r\n");
    });

    // TRANSICIONES LOAD IP
    LoadIp->EstablecerTransiciones( new Transicion*[]{LoadIp2CipMux, nullptr} );

    // LoadIp -> CipMux
    LoadIp2CipMux->EstablecerCondicion([this](){
        if( serialWiFi.ReadChar(&_receivedChar) ) {
            if ( ( _receivedChar != '"' ) &&
            ( _strPositionIndex < IP_MAX_LENGTH ) ) {
                wifiComIpAddress[_strPositionIndex] = _receivedChar;
                _strPositionIndex++;
            } else { 
                return true;
            }
        }
        return false;

    }).EstablecerAccion([this](){
        wifiComIpAddress[_strPositionIndex] = '\0';
        printf("IP address assigned correctly: ");
        printf("%s\r\n\r\n", wifiComIpAddress );
    });

    // TRANSICIONES SEND CIPMUX
    CipMux->EstablecerTransiciones( new Transicion*[]{CipMux2WaitCipMux, nullptr} );
    //CipMux -> WaitCipMux
    CipMux2WaitCipMux->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString( "AT+CIPMUX=1\r\n" );
        _comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // TRANSICIONES WAIT CIPMUX
    WaitCipMux->EstablecerTransiciones( new Transicion*[]{WaitCipMux2CipServer, WaitCipMux2Error, nullptr} );
    // WaitCipMux -> CipServer
    WaitCipMux2CipServer->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // WaitCipMux -> Error
    WaitCipMux2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        printf("AT+CIPMUX=1 command not ");
        printf("responded correctly\r\n");
    });
    
    // TRANSICIONES SEND CIPSERVER
    CipServer->EstablecerTransiciones( new Transicion*[] {CipServer2WaitCipServer, nullptr} );
    // CipServer -> WaitCipServer
    CipServer2WaitCipServer->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString( "AT+CIPSERVER=1,80\r\n" );
        _comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // TRANSICIONES WAIT CIPSERVER
    WaitCipServer->EstablecerTransiciones( new Transicion*[]{WaitCipServer2CipRecvMode, WaitCipServer2Error, nullptr} );
    // WaitCipServer -> CipRecvMode
    WaitCipServer2CipRecvMode->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });
    // WaitCipServer -> ERROR
    WaitCipServer2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        printf("AT+CIPSERVER=1,80 command not ");
        printf("responded correctly\r\n");
    });

    // TRANSICIONES SEND CIPRECVMODE
    CipRecvMode->EstablecerTransiciones( new Transicion*[]{CipRecvMode2WaitCipRecvMode, nullptr} );
    // CipRecvMode -> WaitCipRecvMode
    CipRecvMode2WaitCipRecvMode->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString( "AT+CIPRECVMODE=1\r\n" );
        _comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );
    });



    // TRANSICIONES WAIT CIPRECVMODE
    WaitCipRecvMode->EstablecerTransiciones( new Transicion*[]{ WaitCipRecvMode2CwState/*WaitCipRecvMode2CipStatus*/, WaitCipRecvMode2Error, nullptr} );
    // WaitCipRecvMode -> CwState
    WaitCipRecvMode2CwState->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });
    
    
    
    
    
    // WaitCipRecvMode -> CipStatus
    WaitCipRecvMode2CipStatus->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // WaitCipRecvMode-> Error
    WaitCipRecvMode2Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([](){
        printf("AT+CIPRECVMODE=1 command not ");
        printf("responded correctly\r\n");
    });


    // TRANSICIONES SEND CWSTATE
    

    CipStatus->EstablecerTransiciones( new Transicion*[]{ CipStatus2ProcessingCipStatus, nullptr} );
    // CwState -> ProcessingCwState
    CipStatus2ProcessingCipStatus->EstablecerCondicion([this](){
        return true;
    }).EstablecerAccion([this](){
        flush_last_command_response( serialWiFi );
        serialWiFi.WriteString( "AT+CIPSTATUS\r\n" );
        _delay->Empezar( DELAY_1_SECONDS );
    });

    // TRANSICIONES PROCESSING CIPSTATUS
    ProcessingCipStatus->EstablecerTransiciones(new Transicion*[]{ ProcessingCipStatusActualizacion, ProcessingCipStatus2Error, nullptr });

    ProcessingCipStatusActualizacion->EstablecerAccion([this](){
        if ( _delay->EstaFinalizado() ) {
            if ( wifiStatus == nullptr ) { wifiStatus = new WiFiStatus(); }
            
            switch(wifiStatus->Parse( serialWiFi )) {

                case ParseStatus::FAILED_NO_READ: _maquina->EscribirLog("Fallo\r\n"); break;
                case ParseStatus::SUCCESSFUL_MATCH: _maquina->EscribirLog("Exito\r\n"); break;
                default: break;
            }
        }
    });

    ProcessingCipStatus2CipStatus->EstablecerCondicion([this](){
        return _delay->EstaFinalizado();
    }).EstablecerAccion([this](){

        switch ( wifiStatus->GetState() ) {
            case WiFiConnectionState::CONNECTED_NO_IP:
                _maquina->EscribirLog("Conectado SIN IP\r\n");
                break;
            case WiFiConnectionState::CONNECTED_WITH_IP:
                _maquina->EscribirLog("Conectado con IP\r\n");
                break;
            case WiFiConnectionState::DISCONNECTED:
                _maquina->EscribirLog("Desconectado \r\n");
                break;
            case WiFiConnectionState::CONNECTING:
                _maquina->EscribirLog("Conectando\r\n");
                break;
            case WiFiConnectionState::NOT_STARTED:
                _maquina->EscribirLog("Coneccion no iniciada\r\n");
                break;
            case WiFiConnectionState::UNKNOWN:
                _maquina->EscribirLog("Desconocido.\r\n");
                break;
        }
    });

    ProcessingCipStatus2Error->EstablecerCondicion([this](){
        if ( _delay->EstaFinalizado() ) {
            if ( wifiStatus == nullptr ) { wifiStatus = new WiFiStatus(); }

            //serialWiFi.ReadString(str);
            //_maquina->EscribirLog(str);
            
            switch(wifiStatus->Parse( serialWiFi )) {

                case ParseStatus::FAILED_NO_READ: _maquina->EscribirLog("Fallo\r\n"); break;
                case ParseStatus::SUCCESSFUL_MATCH: _maquina->EscribirLog("Exito\r\n"); break;
                default: break;
            }


            return true;
        }

        return false;

    }).EstablecerAccion([this](){

        switch ( wifiStatus->GetState() ) {
            case WiFiConnectionState::CONNECTED_NO_IP:
                _maquina->EscribirLog("Conectado SIN IP\r\n");
                break;
            case WiFiConnectionState::CONNECTED_WITH_IP:
                _maquina->EscribirLog("Conectado con IP\r\n");
                break;
            case WiFiConnectionState::DISCONNECTED:
                _maquina->EscribirLog("Desconectado \r\n");
                break;
            case WiFiConnectionState::CONNECTING:
                _maquina->EscribirLog("Conectando\r\n");
                break;
            case WiFiConnectionState::NOT_STARTED:
                _maquina->EscribirLog("Coneccion no iniciada\r\n");
                break;
            case WiFiConnectionState::UNKNOWN:
                _maquina->EscribirLog("Desconocido.\r\n");
                break;
        }
    });
    /*
    // TRANSICIONES SEND CIPSTATUS
    CipStatus->EstablecerTransiciones( new Transicion*[]{CipStatus2WaitCipStatusStatus3, nullptr} );
    // CipStatus -> WaitCipStatus3
    CipStatus2WaitCipStatusStatus3->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString( "AT+CIPSTATUS\r\n" );
        _comExpectedResponse = responseStatus3;
        _delay->Empezar( DELAY_1_SECONDS );
    });
    */
/*************************************************************************************************************************************************************************************/




/*************************************************************************************************************************************************************************************/


    // TRANSICION WAIT CIPSTATUS STATUS 3
    WaitCipStatusStatus3->EstablecerTransiciones( new Transicion*[]{WaitCipStatusStatus32WaitCipStatus, WaitCipStatusStatus32CipStatus, nullptr} );
    //  WaitCipStatusStatus3 -> WaitCipStatus
    WaitCipStatusStatus32WaitCipStatus->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
        _comExpectedResponse = responseCipstatus;
    });

    // WaitCipStatusStatus3 -> CipStatus
    WaitCipStatusStatus32CipStatus->EstablecerCondicion([this](){

        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // TRANCISION WAIT CIPSTATUS
    WaitCipStatus->EstablecerTransiciones( new Transicion*[]{WaitCipStatus2WaitGetId, WaitCipStatus2CipStatus, nullptr} );
    // WaitCipStatus -> WaitGetId
    WaitCipStatus2WaitGetId->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([](){});

    // WaitCipStatus -> CipStatus
    WaitCipStatus2CipStatus->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // TRANSICIONES WAIT GET ID
    WaitGetId->EstablecerTransiciones( new Transicion*[]{WaitGetId2WaitCipStatusOk, nullptr} );
    // WaitGetId -> WaitCipStatusOk
    WaitGetId2WaitCipStatusOk->EstablecerCondicion([this](){
        return serialWiFi.ReadChar(&_receivedChar);
    }).EstablecerAccion([this](){
        _currentConnectionId = _receivedChar;
        _comExpectedResponse = responseOk;
    });
    

    // TRANSICIONES WAIT CIPSTATUS OK
    WaitCipStatusOk->EstablecerTransiciones( new Transicion*[]{WaitCipStatusOk2CipRecvDataQuery, nullptr} );
    // WaitCipStatusOk -> CipRecvDataQuery
    WaitCipStatusOk2CipRecvDataQuery->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([](){});

/**********************************/

/* 
    Aca voy a arreglar el código. 

    Tengo que poner el length. 
*/

    // TRANSICIONES SEND CIPRECVLEN QUERY
    CipRecvLenQuery->EstablecerTransiciones( new Transicion*[]{ CipRecvLenQuery2WaitCipRecvLenQuery, nullptr } );
    // CipRecvLenQuery -> WaitCipRecvLenQuery
    CipRecvLenQuery2WaitCipRecvLenQuery->EstablecerCondicion([](){
        return true;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString("AT+CIPRECVLEN?\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );
    });
    
    // TRANSICIONES WAIT CIPRECVDATA QUERY 
    WaitCipRecvLenQuery->EstablecerTransiciones( new Transicion*[]{ WaitCipRecvLenQuery2CipRecvData, WaitCipRecvLenQuery2Error,nullptr} );
    // WaitCipRecvDataQuery -> CipRecvData
    WaitCipRecvLenQuery2CipRecvData->EstablecerCondicion([this](){
        char c;
        char str[50];
        serialWiFi.ReadString( str );
        parseLengthArray(str, linksLengths);
        
        _maquina->EscribirLog(str);
        _maquina->EscribirLog("Evaluando: WaitCipRecvDataQuery2CipRecvData\r\n");
        return false;
    }).EstablecerAccion([this](){});

    // WaitCipRecvDataQuery -> Error
    WaitCipRecvLenQuery2Error->EstablecerCondicion([this](){
        _maquina->EscribirLog("Evaluando: WaitCipRecvDataQuery2Error\r\n");
        return _delay->EstaFinalizado();
    }).EstablecerAccion([this](){

    } );

/**********************************/

    // TRANSICIONES SEND CIPRECVDATA
    CipRecvData->EstablecerTransiciones( new Transicion*[]{CipRecvData2WaitCipRecvData1, nullptr} );
    // CipRecvData -> WaitCipRecvData1
    CipRecvData2WaitCipRecvData1->EstablecerCondicion([](){
        return true;
    }).EstablecerAccion([this](){
        sprintf(_strToSendRecv,"AT+CIPRECVDATA=%c,%d\r\n",_currentConnectionId, HTTP_HEADER_LENGTH);
        sprintf( responseWildcard, "%s%d:",responseCiprecvdata, HTTP_HEADER_LENGTH); // sprintf es lento, es para asegurar que no se pierda datos en wait ciprecvdata
        _comExpectedResponse = responseWildcard;
        serialWiFi.WriteString( _strToSendRecv );
        _delay->Empezar( DELAY_5_SECONDS );
    });

    


    // TRANSICIONES WAIT CIPRECVDATA 1
    WaitCipRecvData1->EstablecerTransiciones( new Transicion*[]{WaitCipRecvData12WaitCipRecvData2, WaitCipRecvData12Error, nullptr} );
    // WaitCipRecvData1 -> WaitCipRecvData2
    WaitCipRecvData12WaitCipRecvData2->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _strPositionIndex=0;
    });

    // WaitCipRecvData1 -> Error
    WaitCipRecvData12Error->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        printf("%s command not ",_strToSendRecv);
        printf("responded correctly\r\n");
        printf("Expected string was: %s\r\n",_comExpectedResponse);
    });


    // TRANSICIONES WAIT CIPRECVDATA 2
    WaitCipRecvData2->EstablecerTransiciones( new Transicion*[]{WaitCipRecvData22CipSend, nullptr} );
    // WaitCipRecvData2 -> CipSend
    WaitCipRecvData22CipSend->EstablecerCondicion([this](){
        if( serialWiFi.ReadChar(&_receivedChar) ) {
            if ( ( _receivedChar != '\n' ) &&
            ( _strPositionIndex < HTTP_HEADER_LENGTH ) ) {
                _strToSendRecv[_strPositionIndex] = _receivedChar;
                _strPositionIndex++;
            } else { 
                return true;
            }
        }
        return false;
    }).EstablecerAccion([this](){
        _strToSendRecv[_strPositionIndex] = '\0';
        _strPositionIndex = 0;
        printf("%s",_strToSendRecv);

    });

    // TRANSICIONES SEND CIPSEND
    CipSend->EstablecerTransiciones( new Transicion*[]{CipSend2WaitCipSend, nullptr} );
    // CipSend -> WaitCipSend
    CipSend2WaitCipSend->EstablecerCondicion([](){
        return true;
    }).EstablecerAccion([this](){
        
        _lengthOfHtmlCode = _app->RenderHtml(this);
        sprintf( _strToSendRecv, "AT+CIPSEND=%c,%d\r\n",_currentConnectionId, _lengthOfHtmlCode );
        serialWiFi.WriteString( _strToSendRecv );
        _comExpectedResponse = responseOk;
    });

    // TRANSICIONES WAIT CIPSEND
    WaitCipSend->EstablecerTransiciones( new Transicion*[]{WaitCipSend2SendHtml, WaitCipSend2CipStatus, nullptr} );
    // WaitCipSend -> SendHtml
    WaitCipSend2SendHtml->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        //printf("WaitCipSend2SendHtml\r\n");
        _delay->Empezar( DELAY_5_SECONDS );
    });
    // WaitCipSend -> CipStatus
    WaitCipSend2CipStatus-> EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        //printf("WaitCipSend2CipStatus\r\n");
        _delay->Empezar( DELAY_10_SECONDS );
    });

    // TRANSICIONES SEND HTML
    SendHtml->EstablecerTransiciones( new Transicion*[]{SendHtml2WaitHtml, nullptr} );
    // SendHtml -> WaitHtml
    SendHtml2WaitHtml->EstablecerCondicion([](){
        return true;
    }).EstablecerAccion([this](){
        _app->SendHtmlTo( this );
        //serialWiFi.WriteString( htmlCode );
        _comExpectedResponse = responseSendOk;
    });


    // TRANSICIONES WAIT HTML
    WaitHtml->EstablecerTransiciones( new Transicion*[]{WaitHtml2CipClose, WaitHtml2CipSend, nullptr} );
    // WaitHtml -> CipClose
    WaitHtml2CipClose->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        //printf("%s\r\n", lastResponse);
        printf("HOLA\r\n");
        _delay->Empezar( DELAY_5_SECONDS );
    });
    // WaitHtml -> CipSend
    WaitHtml2CipSend->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // TRANSICIONES SEND CIPCLOSE
    CipClose->EstablecerTransiciones( new Transicion*[]{CipClose2WaitCipClose, nullptr} );
    // CipClose -> WaitCipClose
    CipClose2WaitCipClose->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        printf("CHAU\r\n");
        sprintf( _strToSendRecv, "AT+CIPCLOSE=%c\r\n", _currentConnectionId );
        serialWiFi.WriteString( _strToSendRecv );
        _comExpectedResponse = responseCipclose;
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // TRANSICIONES WAIT CIPCLOSE
    WaitCipClose->EstablecerTransiciones( new Transicion*[] { WaitCipClose2CipStatus_1, WaitCipClose2CipStatus_2, nullptr } );
    // WaitCipClose->CipStatus_1
    WaitCipClose2CipStatus_1->EstablecerCondicion([this](){
        return _isExpectedResponse();
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });

    // WaitCipClose->CipStatus2
    WaitCipClose2CipStatus_2->EstablecerCondicion([this](){
        return _delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
        _delay->Empezar( DELAY_5_SECONDS );
    });



    maquina->Logs( _logs );

    _maquina = maquina;

    //uartWifi.set_blocking(false);
    serialWiFi.SetBlocking( false );
    

}


void Wifi::ComUpdate(){
    
    _maquina->Evaluar();
}

void Wifi::Logs( bool mostrarLogs ) {
    _logs = mostrarLogs;
    if ( _maquina != nullptr ) {
        _maquina->Logs( _logs );
    }
}

void Wifi::ComRestart(){
    _maquina->Reiniciar();
}

void Wifi::SendHtml(const char* header, const char* content, const char* footer){
    serialWiFi.WriteString( header );
    serialWiFi.WriteString( content );
    serialWiFi.WriteString( footer );
}
// PRIVATE METHODS
/*
bool Wifi::_comCharRead( char* receivedChar ){
    char receivedCharLocal = '\0';
    if( uartWifi.readable() ) {
        uartWifi.read(&receivedCharLocal,1);
        *receivedChar = receivedCharLocal;
        if( _logs ) {
            //_maquina->EscribirLog("     READ: ");
            _maquina->EscribirLog(*receivedChar);
            //_maquina->EscribirLog("\r\n");
        }

        return true;
    }


    return false;
}
*/
/*

void Wifi::_comStringWrite( const char* str ){
    uartWifi.write( str, strlen(str) );
    if ( _logs ) {
        _maquina->EscribirLog("    WRITE: ");
        _maquina->EscribirLog(str);
        _maquina->EscribirLog("\r\n");
    }
}*/
/*
void Wifi::_comStringRead( char* str ){
    
    str[0]='\0';
    
    bool readable = uartWifi.readable();
    char c='\0';

    for( int i = 0; readable; i++ ){
        uartWifi.read(&c, 1);

        if ( c != '\r' && c != '\n' ){
            str[i] = c;
        } else if ( c == '\n'  ){
            str[i] = '\0';
            
            break;
        }
        readable = uartWifi.readable();
        
    }
}
*/
bool Wifi::_isExpectedResponse(){
    static int responseStringPositionIndex = 0;
    char charReceived;
    bool moduleResponse = false;

    if( serialWiFi.ReadChar(&charReceived) ){
        lastResponse[responseStringPositionIndex] = charReceived;
        if ( charReceived == _comExpectedResponse[responseStringPositionIndex] ) {
            responseStringPositionIndex++;
            if (_comExpectedResponse[responseStringPositionIndex] == '\0') {
                responseStringPositionIndex = 0;
                moduleResponse = true;
            }
        } else {
            lastResponse[responseStringPositionIndex+1] = '\0';
            responseStringPositionIndex = 0;
        }
    }
   return moduleResponse;
}

bool Wifi::_isOneOfTheseResponses( const char** responses, int* pos ) {
    
    static int length = 0;
    static bool firstTime = true;
    bool moduleResponse = false;
    char charReceived;

    if ( responses == nullptr || pos == nullptr ) return false;
    
    if ( firstTime ) {
        for (; responses[ length ] != nullptr; length++ );
        indexes = new int[length];
        for (int i = 0; i<length; i++ ) indexes[i] = 0;
        firstTime = false;
    }

    if( serialWiFi.ReadChar(&charReceived) ){
        
        for (int i = 0; i < length; i++) {
            if ( charReceived == responses[i][ indexes[i] ] ) {
                indexes[i]++;
                if ( responses[i][ indexes[i] ] == '\0' ) {
                    //responseStringPositionIndex = 0;
                    moduleResponse = true;
                    firstTime = true;
                    *pos = i;
                    delete[] indexes;
                    indexes = nullptr;
                }
            } else {
                indexes[i] = 0;
            }            
        }

    }
    return moduleResponse;

}