#include "wi-fi.h"

#include "maquinaDeEstados.h"
#include "temporizador.h"
#include "buffer.h"
#include "app.h"
#include "miscellaneous.h"
#include "wi-fi-status.h"
#include "link-queue.h"

#include "wi-fi-commands.h"
#include "uart.h"
#include "at.h"

#include "mbed.h"
#include <cstddef>

#define DELAY_1_SECONDS 1
#define DELAY_5_SECONDS 5
#define DELAY_10_SECONDS 10

#define LINKS_NUMBER 5


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
static char wifiComApSsid[AP_SSID_MAX_LENGTH] = ;
static char wifiComApPassword[AP_PASSWORD_MAX_LENGTH] = ;
static char wifiComIpAddress[IP_MAX_LENGTH];

static const char htmlCode [] =
   "<!DOCTYPE html><html><head><title>SiMBA</title></head><body><p>&nbsp;</p><h1>SiMBa&nbsp;</h1><h2>Estad&iacute;sticas:&nbsp;</h2><ul><li>Temperatura:</li><li>Tapa:</li><li>SensorPir:</li><li>Sensor de gas:</li></ul><h2>Acciones:</h2><ul><li>Abrir tapa</li><li>Cerrar Tapa</li></ul></body></html>";

static char lastResponse[50]="";
static int *indexes = nullptr;

static WiFiStatus* wifiStatus=nullptr;
static WiFiCommands* wifiCommands=nullptr;
static int linksLengths[LINKS_NUMBER];

Wifi::Wifi(App * app){
    _maquina = nullptr;
    _delay = new Temporizador();
   // wifi = &uartWifi;
    _logs = false;
    _rx = nullptr;
    _app = app;
    _route[0] = '\0';
    _route[HTTP_MAX_HEADER_CHARS - 1] = '\0';
    
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
    Estado* WaitCipMuxError = new Estado( "WIFI_STATE_WAIT_CIPMUX_ERROR" );
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
    Estado* WaitCipRecvData = new Estado( "WIFI_STATE_WAIT_CIPRECVDATA" );

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

    
    //Transicion* WaitAt2Idle = new TransicionSimple( Idle );
    
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

    wifiCommands = new WiFiCommands();
    wifiStatus = new WiFiStatus();
    
    // INIT ****************************************

    Init->EstablecerActualizacion( [this, At](){
                
      //  _delay->Empezar(DELAY_5_SECONDS);
        return At;
    });


    // SEND AT *************************************
    At->EstablecerActualizacion([this, WaitAt](){
        Estado * nextState = nullptr;

        //if ( _delay->Estado() == EstadoTemporizador::FINALIZADO ) {
            serialWiFi.WriteString("AT\r\n");
            _comExpectedResponse = responseOk;
            _delay->Empezar(DELAY_5_SECONDS);
            nextState = WaitAt;
            wifiCommands->Reset();
        //}

        return nextState;
    });
    

    // TRANSICIONES WAIT AT *************************************

    WaitAt->EstablecerActualizacion( [this,Error, CwMode](){
        Estado * nextState = nullptr;
        

        switch ( wifiCommands->ParseAtAsync( serialWiFi ) ) {
            case ParseStatus::FAILED_NO_READ: 
                _maquina->EscribirLog("AT command not responded ");
                _maquina->EscribirLog("correctly\r\n");
                nextState = Error;
                break;
            case ParseStatus::SUCCESSFUL_MATCH: 
                nextState = CwMode;
                break;
            default: break;
        }

        return nextState;
    });

    // TRANSICIONES SEND CWMODE
    CwMode->EstablecerActualizacion([this, WaitCwMode](){
        serialWiFi.WriteString("AT+CWMODE=1\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar(DELAY_5_SECONDS);
        return WaitCwMode;
    });

    /*CwMode->EstablecerTransiciones( new Transicion*[]{ CwMode2WaitCwMode, nullptr } );
    // CwMode -> WaitCwMode 
    CwMode2WaitCwMode->EstablecerCondicion([this](){
        return true;
    }).EstablecerAccion([this](){
        serialWiFi.WriteString("AT+CWMODE=1\r\n");
        _comExpectedResponse = responseOk;
        _delay->Empezar(DELAY_5_SECONDS);
    });*/

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
    Cifsr->EstablecerActualizacion([this, WaitCifsr](){
        
        serialWiFi.WriteString( "AT+CIFSR\r\n" );
        wifiCommands->Reset();
        _delay->Empezar(DELAY_5_SECONDS);

        return WaitCifsr;
    });

    /*Cifsr->EstablecerTransiciones( new Transicion*[]{Cifsr2WaitCifsr, nullptr} );
    // Cifsr -> WaitCifsr
    Cifsr2WaitCifsr->EstablecerCondicion([this](){
        return true;//_delay->Estado() == EstadoTemporizador::FINALIZADO;
    }).EstablecerAccion([this](){
            serialWiFi.WriteString( "AT+CIFSR\r\n" );
            _comExpectedResponse = responseCifsr;
            _delay->Empezar(DELAY_5_SECONDS);
    });
*/
    // TRANSICIONES WAIT CIFSR
    WaitCifsr->EstablecerActualizacion([Error, CipMux, this](){
        Estado * nextState = nullptr;

        switch (wifiStatus->ParseCifsrAsync( serialWiFi) ) {
            case ParseStatus::SUCCESSFUL_MATCH: 
                printf("IP: %s\r\n", wifiStatus->Ip());
                printf("MAC: %s\r\n", wifiStatus->Mac());
                nextState = CipMux;
                break;
            case ParseStatus::FAILED_NO_READ:
                printf("AT+CIFSR command not responded ");
                printf("correctly\r\n");
                nextState = Error;
                break;
            default: break;
        }

        return nextState;
    });


    // TRANSICIONES SEND CIPMUX
    CipMux->EstablecerActualizacion([this, WaitCipMux](){
        serialWiFi.WriteString( "AT+CIPMUX=1\r\n" );
        _comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );
        if ( wifiCommands != nullptr ) { wifiCommands->Reset(); }
        

        return WaitCipMux;
    });



    // TRANSICIONES WAIT CIPMUX
    WaitCipMux->EstablecerActualizacion([this, CipServer, Error](){
        Estado * nextState = nullptr;
        if ( wifiCommands == nullptr ) { wifiCommands = new WiFiCommands(); }

        switch (wifiCommands->WaitForOkAsync( serialWiFi )) {
            case ParseStatus::FAILED_NO_READ: 
                _maquina->EscribirLog("AT+CIPMUX=1 command not ");
                _maquina->EscribirLog("responded correctly\r\n");
                nextState = Error;
                break;
            case ParseStatus::SUCCESSFUL_MATCH: 
                _maquina->EscribirLog("Se obtuvo OK CIPMUX=1\r\n");
                _delay->Empezar( DELAY_5_SECONDS );
                nextState = CipServer;
                break;
            default: break;
        }

        return nextState;
    });

    


    // TRANSICIONES SEND CIPSERVER
    CipServer->EstablecerActualizacion([this, WaitCipServer](){
        serialWiFi.WriteString( "AT+CIPSERVER=1,80\r\n" );
        //_comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );

        wifiCommands->Reset(); 

        return WaitCipServer; 
    });



    // TRANSICIONES WAIT CIPSERVER
    WaitCipServer->EstablecerActualizacion([this, CipRecvMode, Error, CipServer](){
        Estado * nextState = nullptr;
        if ( !_delay->EstaFinalizado() ) {
            //if ( !serialWiFi.Readable() ) return nex   if ( wifiCommands == nullptr ) { wifiCommands = new WiFiCommands(); }

            switch ( wifiCommands->WaitForOkAsync( serialWiFi ) ) {
                case ParseStatus::FAILED_NO_READ: 
                    _maquina->EscribirLog("AT+CIPSERVER=1,80 command not ");
                    _maquina->EscribirLog("responded correctly\r\n");
                    nextState = Error;
                    break;
                case ParseStatus::SUCCESSFUL_MATCH: 
                    //_maquina->EscribirLog("WaitCiPServer_SUCCESS\r\n");
                    nextState = CipRecvMode;
                    break;        
                default: 
                    break;
            }
        } else nextState = CipServer;
        
        return nextState;
    });
    

    // TRANSICIONES SEND CIPRECVMODE
    CipRecvMode->EstablecerActualizacion([this, WaitCipRecvMode ](){
        serialWiFi.WriteString( "AT+CIPRECVMODE=1\r\n" );
        
        _delay->Empezar( DELAY_5_SECONDS );
        return WaitCipRecvMode;
    });

    // TRANSICIONES WAIT CIPRECVMODE
    WaitCipRecvMode->EstablecerActualizacion([this, Error, CipStatus](){
        Estado * nextState = nullptr;

        if ( !serialWiFi.Readable() ) return nextState;

        if ( wifiCommands == nullptr ) { wifiCommands = new WiFiCommands(); }

        switch ( wifiCommands->ParseCipRecvMode( serialWiFi )) {
            case ParseStatus::FAILED_NO_READ: 
                _maquina->EscribirLog("AT+CIPRECVMODE=1\r\n command not ");
                _maquina->EscribirLog("responded correctly\r\n");
                nextState = Error;
                break;
            case ParseStatus::SUCCESSFUL_MATCH: 
                switch ( wifiCommands->GetState() ) {

                    case WiFiCommandState::OK:
                        _maquina->EscribirLog("Se obtuvo OK AT+CIPSERVER=1,80\r\n");
                        _delay->Empezar( DELAY_5_SECONDS );
                        nextState = CipStatus;
                        break;
                    default: 
                        _maquina->EscribirLog("No se obtuvo respuesta\r\n");
                        break;   
                }
                break;
            default: break;
            }
            return nextState;
    });


    // SEND CWSTATE
    CipStatus->EstablecerActualizacion([this, ProcessingCipStatus](){
        flush_last_command_response( serialWiFi );
        serialWiFi.WriteString( "AT+CIPSTATUS\r\n" );
       _delay->Empezar( DELAY_1_SECONDS );


        // Test
        //_app->RenderHtml( this );

        wifiStatus->ResetIndex();
        return ProcessingCipStatus;
    });

    // PROCESSING CIPSTATUS
    ProcessingCipStatus->EstablecerActualizacion([this, CipStatus, CipRecvLenQuery](){

        Estado * nextState = nullptr;
        
        if ( _delay->EstaFinalizado() ) {return CipStatus;}
           // nextState = CipStatus;
            
            //if (!serialWiFi.Readable()) { return nextState;}
            switch(wifiStatus->ParseAsync( serialWiFi )) {

                case ParseStatus::FAILED_NO_READ: _maquina->EscribirLog("Fallo\r\n"); break;
                case ParseStatus::SUCCESSFUL_MATCH:
                    
                    switch ( wifiStatus->GetState() ) {
                       case WiFiConnectionState::CONNECTED_NO_IP:
                           //_maquina->EscribirLog("Conectado SIN IP\r\n");
                           break;
                       case WiFiConnectionState::CONNECTED_WITH_IP:
                           //_maquina->EscribirLog("Conectado con IP\r\n");
                          // _app->SendHtmlTo( this );

                           break;
                       case WiFiConnectionState::DISCONNECTED:
                           //_maquina->EscribirLog("Desconectado \r\n");
                         //  _app->SendHtmlTo( this );
                           break;
                       case WiFiConnectionState::CONNECTING:
                           //_maquina->EscribirLog("Conectando\r\n");
                           nextState = CipRecvLenQuery;
                           break;
                       case WiFiConnectionState::NOT_STARTED:
                           //_maquina->EscribirLog("Coneccion no iniciada\r\n");
                           break;
                       case WiFiConnectionState::UNKNOWN:
                           //_maquina->EscribirLog("Desconocido.\r\n");
                           break;
                    }
                
                    break;
                
                default:
                    
                 break;
            }
            
       // }

        return nextState;

    });

    // TRANSICIONES SEND CIPRECVLEN QUERY
    CipRecvLenQuery->EstablecerActualizacion([this, WaitCipRecvLenQuery](){
        serialWiFi.WriteString("AT+CIPRECVLEN?\r\n");
        _maquina->EscribirLog("Enviado CIPRCVLEN\r\n");
        wifiStatus->ResetIndex();
        return WaitCipRecvLenQuery;

    });

    
    // TRANSICIONES WAIT CIPRECVDATA QUERY 
    WaitCipRecvLenQuery->EstablecerActualizacion([this, Error, CipRecvData](){
        Estado * nextState = nullptr;
        ParseStatus st;
        switch ( st = wifiStatus->ParseCipRecvLenAsync( serialWiFi ) ) {
            case ParseStatus::FAILED_NO_READ:
                _maquina->EscribirLog("No se parseo la longitud de los links\r\n"); 
                nextState = Error;
                break;
            case ParseStatus::SUCCESSFUL_MATCH: 
                _maquina->EscribirLog("Se parseo el linklength\r\n"); 
                nextState = CipRecvData;
                break;
            default: 
                //printf("FALLO LINKLEGNTH %d\r\n", (int)st);
            break;
        }
        return nextState;
    });


    // SEND CIPRECVDATA
    CipRecvData->EstablecerActualizacion([this, WaitCipRecvData, CipStatus, CipClose](){
        
        Estado * nextState = nullptr;
        _currentConnectionId = wifiStatus->GetLink();
        if (_currentConnectionId != -1) {
            printf("EL LINK: %d\r\n",_currentConnectionId);
            printf("EL LENGTH: %d\r\n",wifiStatus->GetLinkLenght(_currentConnectionId));
            if ( wifiStatus->GetLinkLenght(_currentConnectionId) != 0) {

                sprintf(_strToSendRecv,"AT+CIPRECVDATA=%d,%d\r\n", _currentConnectionId, HTTP_HEADER_LENGTH);
                sprintf( responseWildcard, "%s%d:",responseCiprecvdata, HTTP_HEADER_LENGTH); // sprintf es lento, es para asegurar que no se pierda datos en wait ciprecvdata

                serialWiFi.WriteString( _strToSendRecv );
                _delay->Empezar( DELAY_5_SECONDS );

                wifiCommands->Reset();
                nextState = WaitCipRecvData;
            } else nextState = CipClose;
            
        } else {
            nextState = CipStatus; 
        }
        return nextState;
    });

    
    // ESTADO WAIT CIPRECVDATA
    WaitCipRecvData->EstablecerActualizacion([this, CipSend, Error](){
        Estado * nextState = nullptr;
        //printf("WAIT CIP RECV DATA\r\n");
        switch( wifiCommands->ParseCipRecvDataAsync( serialWiFi, _route, HTTP_MAX_HEADER_CHARS) ) {
            case ParseStatus::SUCCESSFUL_MATCH:
                nextState = CipSend;
                _trimRoute();
                
                _lengthOfHtmlCode = _app->HandleRequest(this);
                break;
            case ParseStatus::FAILED_NO_READ:
                nextState = Error;
                break;
            default: 
                //printf("My String: %s\r\n", _route);
            
            break;
        }
        return nextState;
    });

    

    // TRANSICIONES SEND CIPSEND
    CipSend->EstablecerActualizacion([this, WaitCipSend](){
        int htmlLength;
        if ( _lengthOfHtmlCode > TCP_IP_MAX_SEND_CHARS ) {
            htmlLength = TCP_IP_MAX_SEND_CHARS;
        } else {
            htmlLength = _lengthOfHtmlCode;
        }

        sprintf( _strToSendRecv, "AT+CIPSEND=%d,%d\r\n", _currentConnectionId, htmlLength );
        serialWiFi.WriteString( _strToSendRecv );
        _comExpectedResponse = responseOk;
        _delay->Empezar( DELAY_5_SECONDS );
        wifiCommands->Reset();
        return WaitCipSend;
    });


    // TRANSICIONES WAIT CIPSEND
    WaitCipSend->EstablecerActualizacion([this, SendHtml, CipStatus,Error](){
        Estado * nextState = nullptr;

        if (_delay->Estado() != EstadoTemporizador::FINALIZADO ) {
            switch(wifiCommands->ParseCipSendAsync( serialWiFi )) {
            case ParseStatus::SUCCESSFUL_MATCH:
                printf("WAIT CIP SEND SUCCESSFUL\r\n");
                nextState = SendHtml;
                break;
            case ParseStatus::FAILED_NO_READ:
                printf("WAIT CIP SEND ERROR\r\n");
                nextState = Error;
                break;
            default: 
            
            
            break;
            }
        } else {
            nextState = CipStatus;
        }
        
        return nextState;
    });

    // TRANSICIONES SEND HTML
    SendHtml->EstablecerActualizacion([this, WaitHtml, At](){
        int htmlLength;
        if ( _lengthOfHtmlCode > TCP_IP_MAX_SEND_CHARS ) {
            htmlLength = TCP_IP_MAX_SEND_CHARS;
        } else {
            htmlLength = _lengthOfHtmlCode;
        }

        printf("hola\n");
        _app->SendHtml( this , htmlLength);
        _lengthOfHtmlCode -= htmlLength; 
        //_currentConnectionId =0;
        //_comExpectedResponse = responseSendOk;
        wifiCommands->Reset();
        printf("Saliendo SendHtml\r\n");
        return WaitHtml;//WaitHtml;
    });

    WaitHtml->EstablecerActualizacion([this, CipClose,CipSend, Error](){
        Estado * nextState = nullptr;
        switch( wifiCommands->WaitForCipSendOkAsync( serialWiFi ) ) {
             case ParseStatus::SUCCESSFUL_MATCH:
             
                if ( _lengthOfHtmlCode == 0 ) {
                    nextState = CipClose;
                } else {
                    nextState = CipSend; 
                }
                
                break;
            case ParseStatus::FAILED_NO_READ:
                
                nextState = Error;
                break;
            default: 
            break;

        }
        return nextState;
    });

    // TRANSICIONES SEND CIPCLOSE
    CipClose->EstablecerActualizacion([this, WaitCipClose](){
        sprintf( _strToSendRecv, "AT+CIPCLOSE=%d\r\n", _currentConnectionId );
        serialWiFi.WriteString( _strToSendRecv );
        
        _delay->Empezar( DELAY_5_SECONDS );

        wifiCommands->Reset();

        return WaitCipClose;
    });


    // TRANSICIONES WAIT CIPCLOSE
    WaitCipClose->EstablecerActualizacion([this, Error, CipStatus, CipClose](){
        Estado * nextState = nullptr;
        if ( !_delay->EstaFinalizado() ) {

            switch( wifiCommands->WaitForOkAsync( serialWiFi ) ) {
                case ParseStatus::SUCCESSFUL_MATCH:
                    nextState = CipStatus;
                    break;
                case ParseStatus::FAILED_NO_READ:
                    nextState = Error;
                    break;
                default: 
                    break;
            }
        } else { 
            nextState = CipClose;
        }
        
        return nextState;
    });

    // Error
    Error->EstablecerActualizacion([this, At](){

        _delay->Empezar(DELAY_5_SECONDS );

        return At;
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

void Wifi::SendHtml( const char* content ){
    
    serialWiFi.WriteString( content );

}

void Wifi::SendHtmlN( const char* content, int length ){
    
    serialWiFi.WriteStringN( content , length);

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

void Wifi::_trimRoute() {
    if (_route[0] == '\0') {
        return;
    }

    int count = 0;

    for (int i = 0; i < HTTP_MAX_HEADER_CHARS; i++) {
        if (_route[i] == '\0') {
            break;
        }

        if (_route[i] == ' ') {
            count++;
            if (count == 2) {
                _route[i] = '\0'; 
                break;
            }
        }
    }
}

char * Wifi::Route() {
    return _route;
}