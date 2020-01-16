//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UThrComSerial.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TThrComSerial::TThrComSerial(bool CreateSuspended, String comPort, String baudrate)
   : TThread(CreateSuspended)
{
   onAtualizacaoDados = NULL;
   Com = NULL;
   StatusATM.comOnline(0);
   StatusATM.Com.comPort = comPort;
   StatusATM.Com.baudrate = baudrate;

   Com = new TComSerial(Com, StatusATM.Com.comPort, StatusATM.Com.baudrate.ToInt());
   Com->Open();
   //Com->DTROFF();
   StatusATM.comOnline(1);

   StatusATM.pedidoEstadoRele2 = 255;
   StatusATM.pedidoEstadoRele3 = 255;
}
//---------------------------------------------------------------------------
__fastcall TThrComSerial::~TThrComSerial()
{
   if(Com != NULL) delete Com;
}
//---------------------------------------------------------------------------
void __fastcall TThrComSerial::Execute()
{
   //---- Thread Start ----
   while(!Terminated)
   {               
      //tratar dados serial de 1 em 1s
      trataDados();

      //Aguarda 1s e a cada 10ms verifica pedidos
      for(int i = 0; i < 10 && !Terminated; i++)
      {
         Sleep(100);
         //verifica
         if(StatusATM.pedidoFirmware == 1)
            firmware();
         if(StatusATM.pedidoReset > 0)
            reset();
         if(StatusATM.pedidoRele2 == 1 || StatusATM.pedidoRele3 == 1)
            rele();
      }
   }
}
//---------------------------------------------------------------------------
void __fastcall TThrComSerial::trataDados()
{
   if(onAtualizacaoDados != NULL)   Synchronize(onAtualizacaoDados);
}
//---------------------------------------------------------------------------
int __fastcall TThrComSerial::sendRecv(unsigned char cmd, const void* dados, int length, void* dados_rec, int dados_rec_len_max, int* dados_rec_len)
{
   int tamanho_enviar;
   //foram declarados globalmente
   //TAtmMonRequest request;
   //TAtmMonResult result;
   memset(&request, 0, sizeof(request));
   memset(&result, 0, sizeof(result));
   TVarRec arg;

   if ( length > MAX_ATMMON_DATA_LENGTH ) {
      length=MAX_ATMMON_DATA_LENGTH;      // guardachuva
   }

   sequencia++;

   request.iniciador = STX;
   request.sequencia = sequencia;
   request.comando = cmd;
   request.length = (unsigned char)length;
   memcpy(request.dados, dados, length);

   tamanho_enviar = sizeof(TAtmMonRequest) + length - MAX_ATMMON_DATA_LENGTH;

   //calcular CRC
   //------------------------------------------------------
   unsigned short crc = calccrc16(&request, tamanho_enviar-2);
   unsigned char* tmp = (unsigned char*)&request;
   memcpy(&tmp[tamanho_enviar-2], &crc, 2);

   for ( int retry=0 ; retry < 3 ; retry++ ) {
      int r;
      Com->Write(&request, tamanho_enviar);
      r = monta_pacote(&result, request.comando, request.sequencia);
      if ( r == 0  )
      {
         int len = (int)result.length;
         if (dados_rec_len != NULL)*dados_rec_len=len;
         if (dados_rec != NULL && dados_rec_len_max > 0 )
         {
            if (len > dados_rec_len_max)len = dados_rec_len_max;
            memcpy(dados_rec, result.dados,len);
         }
         //*struct recebida*
         return (int)result.status;
      }
      StatusATM.info = "CMD " + IntToHex(cmd,2) + " rslt=" + String(r) + " retry=" + String(retry);
      //Escreve no memo Com do main form
      if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
   }
   //*erro*
   StatusATM.info = "CMD " + IntToHex(cmd,2) + " ********* FALHOU *******";
   //Escreve no memo Com do main form
   if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
   StatusATM.comOnline(0);
   //Atualiza status bar main
   StatusATM.Com.comunicacao = "Comunicação: OFFLINE";
   StatusATM.Com.uptime = "";
   StatusATM.Com.sequencia = "";
   StatusATM.Com.status = "";
   StatusATM.Com.sensores = "";
   //Escreve no status bar do form main
   if(onAtualizaStatus != NULL)   Synchronize(onAtualizaStatus);
   return -1;
}
//---------------------------------------------------------------------------  
int __fastcall TThrComSerial::monta_pacote(TAtmMonResult* pkt, unsigned char cmd, unsigned char sequencia)
{
   int byte_recebido;
   unsigned char* buffer;
   unsigned idx;
   unsigned tamanho_minimo;
   memset(pkt, 0, sizeof(TAtmMonResult));
   buffer = (unsigned char*)pkt;
   unsigned long tempo = MS_TIMER;
   idx = 0;
   tamanho_minimo = sizeof(TAtmMonResult) - MAX_ATMMON_DATA_LENGTH;
   for (;;) {
      byte_recebido = Com->Getc();
      if (byte_recebido != -1)
      {
         if (idx == 0 && byte_recebido == NAK)
         {
            StatusATM.info = "NAK RECEBIDO";
            //Escreve no memo Com do main form
            if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
            return NAK;
         }
         else if (idx == 0 && byte_recebido == ACK) {
            buffer[idx] = (unsigned char)byte_recebido;
            idx++;
         }
         else if (idx == 0)
         {
            StatusATM.info = "Byte recebido INVALIDO " + IntToHex(byte_recebido,2);
            //Escreve no memo Com do main form
            if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
         }
         else {
            buffer[idx] = (unsigned char)byte_recebido;
            idx++;
            if (idx >= tamanho_minimo) {
               if (idx == (tamanho_minimo+(unsigned)pkt->length))
               {
                  //Verificar o CRC
                  //--------------------------------------------------
                  unsigned short crc_calculado = calccrc16(buffer, idx-2);
                  unsigned short crc_recebido;
                  memcpy(&crc_recebido, &buffer[idx-2], 2);
                  if(crc_recebido != crc_calculado)
                  {
                     StatusATM.info = "crc recebido: " + IntToHex(crc_recebido,4) + " crc calculado: " + IntToHex(crc_calculado,4);
                     //Escreve no memo Com do main form
                     if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
                     return -2;
                  }
                  if (pkt->sequencia != sequencia || pkt->comando != cmd)
                  {
                     StatusATM.info = "Sequencia: " + IntToHex(sequencia, 2) + " / Recebido sequencia: " + IntToHex(pkt->sequencia,2);
                     //Escreve no memo Com do main form
                     if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
                     StatusATM.info = "Comando: " + IntToHex(cmd, 2) + " / Recebido comando: " + IntToHex(pkt->comando,2);
                     //Escreve no memo Com do main form
                     if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
                     StatusATM.info = "pacote descartado sequencia/comando incorretos";
                     //Escreve no memo Com do main form
                     if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
                     idx=0;
                  }
                  else {
                     return 0;
                  }
               }
            }
         }
      }
      if (idx == 0 && (MS_TIMER-tempo) > 200)
      {
         StatusATM.info = "NENHUM BYTE RECEBIDO";
         //Escreve no memo Com do main form
         if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
         return -1;
      }
      if ( (MS_TIMER-tempo) > 500 ) {
         char txt[300];
         memset(txt, 0, sizeof(txt));
         BinToHex((char*)buffer, txt, idx);
         StatusATM.info = "PACOTE INVALIDO " + String(idx) + " bytes: " + String(txt);
         //Escreve no memo Com do main form
         if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
         StatusATM.info = "Estourou com: " + String(MS_TIMER-tempo);
         //Escreve no memo Com do main form
         if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
         return -1;
      }
   }
}
//---------------------------------------------------------------------------
void __fastcall TThrComSerial::reset()
{
   if(StatusATM.pedidoReset == 1)// pedido reset via firmware
      sendRecv(0xAA, NULL, 0, NULL, 0, NULL);
   if(StatusATM.pedidoReset == 2)// pedido reset via serial
   {
      Com->DTRON();
      Sleep(250);
      Com->DTROFF();
   }
   if(StatusATM.pedidoReset == 3)// pedido reset POWER PC
   {
      int r = sendRecv(0x03, &StatusATM.tempoBoot, 0x02, NULL, 0, NULL);
      if(r == 0)
      {
         StatusATM.info = "PC POWER reset OK";
         //Escreve no memo Com do main form
         if(onAtualizaMemoCom != NULL)   Synchronize(onAtualizaMemoCom);
      }
      else
      {
         StatusATM.info = "PC POWER reset ERRO: " + String(r);
         //Escreve no memo Com do main form
         if(onAtualizaMemoCom != NULL)
            Synchronize(onAtualizaMemoCom);
      }
   }
   //volta status requestReset para zero
   StatusATM.requestReset(0);
}

