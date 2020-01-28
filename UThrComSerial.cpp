//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UThrComSerial.h"
// sprintf
#include <stdio.h>
//---------------------------------------------------------------------------
__fastcall TThrComSerial::TThrComSerial(bool CreateSuspended, String comPort, int baudrate)
   : TThread(CreateSuspended)
{
   Comm = NULL;

   BufferTX="";
   BufferRX="";
   LastError="";
   LastMessage="";

   // Estados
   Enviar = false;
   STThread = stIdle;

   // Eventos
   onAtualizacaoDados = NULL;

   ComPort=comPort;
   BaudRate=baudrate;

   bool r=OpenSerialPort(ComPort, BaudRate);
   if(r == false){
      LastError="Fail to open comport=" + ComPort + ", baudrate=" + String(BaudRate);
   }
   else{
      LastMessage="Success opening comport=" + ComPort + ", baudrate=" + String(BaudRate);
   }
}
//---------------------------------------------------------------------------
__fastcall TThrComSerial::~TThrComSerial()
{
   if(Comm != NULL && Comm->Enabled()){
      CloseSerialPort();
   }

   if(Comm != NULL){
      delete Comm;
      Comm = NULL;
   }
}
//---------------------------------------------------------------------------
void __fastcall TThrComSerial::SetSTThread(TSTThread value)
{
   FSTThread = value;
}
//---------------------------------------------------------------------------
void __fastcall TThrComSerial::Execute()
{
   int r = 0;
   String output = "";

   unsigned char bufferRX[TAM_MAX_BUFFER]; // Buffer RX
   memset(bufferRX, 0, sizeof(bufferRX));
   int len_rx = 0; // tamanho Buffer RX

   //---- Thread Start ----
   while(!Terminated)
   {
      if(LastError != "" || LastMessage != "")
      {
         if(onAtualizacaoDados != NULL){
            Synchronize(onAtualizacaoDados);
         }
         LastError="";
         LastMessage="";
      }

      if(Enviar == true){
         STThread = stRunning;

         r = SendRcv(BufferTX, &BufferRX);
         if(r == RX_BUFFER_TOUT){
            // TODO ... on timeout Comm
         }
         else{
            if(onAtualizacaoDados != NULL){
               Synchronize(onAtualizacaoDados);
            }
         }

         BufferRX = "";

         STThread = stIdle;

         Enviar = false;
         CancelaRcv = false;
      }

      Sleep(1); // para liberar processamento
   }
}
//---------------------------------------------------------------------------
void __fastcall TThrComSerial::Send(String input)
{
   Enviar=true;
   BufferTX=input;
}
//---------------------------------------------------------------------------
String __fastcall TThrComSerial::BufferToString(const void *_buffer, int len, bool forceHex)
{
   /* Caso enviado bool forceHex=true, o LogBuffer imprime todos em bytes em formato <hexadecimal> */

   unsigned char* buffer = (unsigned char*)_buffer;
   String str = "";
   char tmp[8];
   for(int i=0;i<len;i++){
      if(forceHex){
         sprintf(tmp, "<%02X>", (int)buffer[i]);
      }
      else{
         if(buffer[i] < ' ' || buffer[i] > 'z'){
            sprintf(tmp, "<%02X>", (int)buffer[i]);
         }
         else{
            memset(tmp, 0, sizeof(tmp));
            tmp[0] = buffer[i];
         }
      }
      str=str+tmp;
   }
   return str;
}
//---------------------------------------------------------------------------
bool __fastcall TThrComSerial::OpenSerialPort(String comPort, int baudRate)
{
   if(Comm != NULL){
      return false;
   }
   else{
      try{
         Comm = new TComSerial(Comm, comPort, baudRate);
         Comm->Open();
         return true;
      }
      catch(Exception& e)
      {
         if(Comm != NULL)
         {
            delete Comm;
            Comm=NULL;
         }
         LastError="Fail to open comport=" + ComPort + ", baudrate=" + String(BaudRate);
         return false;
      }
   }
}
//---------------------------------------------------------------------------
bool __fastcall TThrComSerial::CloseSerialPort()
{
    if(Comm == NULL){
        return true;
    }
    else{
        Comm->Close();
        delete Comm;
        Comm = NULL;
    }
    return false;
}
//---------------------------------------------------------------------------
int __fastcall TThrComSerial::CheckComm()
{
   int rslt = 0;
   try{
      if(Comm == NULL){
         rslt = COM_NOT_OPEN;
      }
      else if(Comm != NULL && Comm->Enabled()){
         rslt = COM_OK;
      }
   }
   catch(Exception &e){
      rslt = COM_ERROR;
   }
   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrComSerial::SendRcv(String input, String *output)
{
   unsigned char bufferRX[TAM_MAX_BUFFER]; // Buffer RX
   memset(bufferRX, 0, sizeof(bufferRX));
   int len_input=input.Length();
   int len_rx=0;
   int rslt = 0;

   try{
      rslt = CheckComm();
      if(rslt == COM_OK)
      {
         // Tenta envio/recebimento por x RETRIES com TIMEOUT_RX
         for(int i=0; i < RETRIES; i++)
         {
            Comm->Write(input.c_str(), len_input);
            rslt = RcvComm(bufferRX, &len_rx);
            if(rslt != RX_BUFFER_TOUT){
               *output = BufferToString(&bufferRX, len_rx);
               break;
            }
            else{
               // TODO... timeout
            }
         }
      }
   }
   catch(Exception &e){
      rslt = COM_ERROR;
   }

   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrComSerial::RcvComm(unsigned char *buffer, int *len_rx, unsigned long msTimeout)
{
   int byte_recebido = 0;
   int idx = 0;
   unsigned long tempo = MS_TIMER;

   memset(buffer, 0, sizeof(buffer));
   *len_rx = 0;

   while(true)
   {
      byte_recebido = Comm->Getc();

      // Se byte_recebido = -1, nao ha bytes para leitura
      if(byte_recebido != -1){
         buffer[idx++] = (unsigned char)byte_recebido;
      }
      else{
         *len_rx=idx;
         return RX_BUFFER_OK;
      }

      // Se nao recebeu nenhum byte e excedeu tempo timeout
      if (idx == 0 && MS_TIMER-tempo > msTimeout){
         return RX_BUFFER_TOUT;
      }
   }
}
//---------------------------------------------------------------------------
