#include "wi-fi.h"

#include "maquinaDeEstados.h"
#include "mbed.h"

UnbufferedSerial uartWifi( PE_8, PE_7, 115200 );

Wifi::Wifi(){
    _maquina = nullptr;
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
    Transicion* WaitAt2Error = new Transicion( Error );
    Transicion* CwMode2WaitCwMode = new Transicion( WaitCwMode );
    Transicion* WaitCwMode2CwJapIsSet = new Transicion( CwJapIsSet );
    Transicion* WaitCwMode2Error = new Transicion( Error );
    

}
