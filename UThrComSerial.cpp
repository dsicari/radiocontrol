//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UThrComSerial.h"
// sprintf
#include <stdio.h>

#pragma link "lib_uteis.lib"
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TThrSerialCom::TThrSerialCom(bool CreateSuspended)
   : TThread(CreateSuspended)
{
   Comm = NULL;

   // Estados
   EnviarCmd = false;
   STThread = stIdle;

   // Eventos
   onAtualizacaoDados = NULL;
}
//---------------------------------------------------------------------------
__fastcall TThrSerialCom::~TThrSerialCom()
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
void __fastcall TThrSerialCom::SetCmd2Send(String value)
{
   FCmd2Send = value;
}
//---------------------------------------------------------------------------
void __fastcall TThrSerialCom::SetSTThread(TSTThread value)
{
   FSTThread = value;
}
//---------------------------------------------------------------------------
void __fastcall TThrSerialCom::SetSTBlocante(int value)
{
   FSTBlocante = value;
}
//---------------------------------------------------------------------------
void __fastcall TThrSerialCom::Execute()
{
   int r = 0;
   String output = "";

   unsigned char bufferRX[TAM_MAX_BUFFER]; // Buffer RX
   memset(bufferRX, 0, sizeof(bufferRX));
   int len_rx = 0; // tamanho Buffer RX

   //---- Thread Start ----
   while(!Terminated)
   {
      if(EnviarCmd == true){
         STThread = stRunning;
         STBlocante = PP_PROCESSING;

         // Gera evento FBlocante, neste evento TAbecs consulta Cmd2Send para saber comando enviado
         if(OnFuncaoBlocante != NULL){
            Synchronize(OnFuncaoBlocante);
         }

         // TEMPO_FBLOCANTE * 10 * TIMEOUT_THR_RX = Tempo para receber informacao do pinpad quando em funcao blocante
         for(int i = 0; i < TEMPO_FBLOCANTE * 10 && !CancelaRcv; i++){
            r = RcvComm(bufferRX, &len_rx, TIMEOUT_THR_RX);
            if(r != RX_BUFFER_TOUT){
               //Disponibilizar bufferRX como string descontando SYN, ETB e CRC
               StrBufferRX = BufferToString(&bufferRX[1], len_rx - 3);
               STBlocante = StrBufferRX.SubString(4,3).ToIntDef(ERROR_ABECS);
               break;
            }
            else{
               // Nao faz nada, ja sinalizou que esta em PP_PROCESSING
            }
         }

         STThread = stIdle;

         // Gera evento FBlocante, neste evento TAbecs consulta Cmd2Send para saber comando enviado
         if(OnFuncaoBlocante != NULL && CancelaRcv == false){
            Synchronize(OnFuncaoBlocante);
         }

         EnviarCmd = false;
         CancelaRcv = false;
         Cmd2Send = "";
         Msg2Send = "";  
      }

      Sleep(1); // para liberar processamento
   }
}
//---------------------------------------------------------------------------
UINT16 __fastcall TThrSerialCom::CRC_Calc(unsigned char *pbData, int iLength)
{
   /* DISPONIVEL BC_v1.08a pag 111/119 */

   UINT16 wData, wCRC = 0;
   int i;
   for ( ;iLength > 0; iLength--, pbData++) {
      wData = (UINT16) (((UINT16) *pbData) << 8);
      for (i = 0; i < 8; i++, wData <<= 1) {
         if ((wCRC ^ wData) & 0x8000)
            wCRC = (UINT16) ((wCRC << 1) ^ CRC_MASK);
         else
            wCRC <<= 1;
      }
   }
   return wCRC;
}
//---------------------------------------------------------------------------
String __fastcall TThrSerialCom::BufferToString(const void *_buffer, int len, bool forceHex)
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
bool __fastcall TThrSerialCom::OpenSerialPort(String comPort, int baudRate)
{
   if(Comm != NULL){
      return;
   }
   else{
      try{
         Comm = new TComSerial(Comm, comPort, baudRate);
         Comm->Open();
      }
      catch(Exception& e)
      {
         if(Comm != NULL)
         {
            delete Comm;
            Comm=NULL;
         }
      }
   }
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::CloseSerialPort()
{
    if(Comm == NULL){
        return;
    }
    else{
        Comm->Close();
        delete Comm;
        Comm = NULL;
    }
    return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::CheckComm()
{
   int rslt = 0;
   try{
      if(Comm == NULL){
         rslt = PP_NOTOPEN;
      }
      else if(Comm != NULL && Comm->Enabled()){
         rslt = PP_OK;
      }
   }
   catch(Exception &e){
      rslt = PP_COMMERR;
   }
   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::Abort()
{
   int rslt = 0;

   if(STThread == stRunning)
   {
      CancelaRcv = true;
      int countCancelaRcv = 0;

      while(CancelaRcv == true){ // Aguardar terminar/cancelar EnviarCmd no Execute da thread, aguardar por 100ms
         Sleep(10);
         countCancelaRcv++;
         if(countCancelaRcv == 10){
            countCancelaRcv = 0;
            rslt = ERROR_ABECS;
            break;
         }
      }
   }

   if(rslt != ERROR_ABECS){
      rslt = SendAbort();
      // Se recebeu EOT entao retornar PP_OK
      if(rslt == EOT_RCV){
         rslt = PP_OK;
      }
   }

   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::SendAbort()
{
   int rslt = 0;
   int byte_recebido = 0;
   unsigned long tempo = MS_TIMER;
   int countTimer = 0;

   try
   {
      if(Comm != NULL){
         // Tenta envio por 3x com timeout de 2s cada
         for(int i = 0; i<=3 && countTimer<3; i++)
         {
            // Enviar CAN (24 decimal)
            Comm->Putc(24);

            while(true)
            {
               byte_recebido = Comm->Getc();

               // Aguardamos apenas 1 byte e deve ser EOT (Retorno do CAN enviado)
               if(byte_recebido != -1){
                  if(byte_recebido == EOT){
                     return EOT_RCV;
                  }
                  else{
                     break;
                  }
               }
               if(MS_TIMER-tempo > TIMEOUT_RX){
                  tempo = MS_TIMER;
                  countTimer++;
                  break;
               }
            }

            if(countTimer == 3){
               countTimer = 0;
               rslt = RX_BUFFER_TOUT;
               break;
            }
         }
      }
   }
   catch(Exception &e){
      rslt = ERROR_ABECS;
   }

   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::Send(String cmd, String msg, bool retornarUltimaFBlocante)
{
   // retornarUltimaFBlocante serve para aguardar buffer do pinpad novamente
   // apos ultima FBlocante ter recebido PP_NOTIFY. Neste caso pula etapa de receber ACK

   // cmd possui 3 caracteres?
   if(cmd.Length() > 3 || cmd.Length() == 0){
      return ERROR_CMD;
   }

   int rslt = 0;
   String output = "";

   if(retornarUltimaFBlocante == false){
      rslt = SendRcv(cmd, msg, &output, true);
      if(rslt == ACK_RCV){
         Cmd2Send = cmd;
         Msg2Send = msg;
         EnviarCmd = true;
         rslt = PP_OK;
      }
   }
   else{
      Cmd2Send = cmd;
      Msg2Send = msg;
      EnviarCmd = true;
      rslt = PP_OK;
   }

   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::SendRcv(String cmd, String msg, String *output, bool receberApenasACK)
{
   String strLenMsg; // tamanho msg
   int lenMsg = 0;
   int r = 0;
   int len = 0;   // posicao/tamanho buffer
   int len_rx = 0;
   unsigned short crc = 0; // Calculo crc
   unsigned char bufferRX[TAM_MAX_BUFFER]; // Buffer RX
   memset(bufferRX, 0, sizeof(bufferRX));
   unsigned char bufferTX[TAM_MAX_BUFFER]; // Buffer TX
   memset(bufferTX, 0, sizeof(bufferTX));

   // cmd possui 3 caracteres?
   if(cmd.Length() > 3 || cmd.Length() == 0){
      return ERROR_CMD;
   }

   // msg não pode ser maior que [DADOS_MSG(total) - CMD - LENGTH] = 1024 - 3 - 3 = 1018 bytes
   // porem é limitada a 999 caracteres pelo proprio LENGTH
   // logo tamanho buffer [SYN(1) + CMD(3) + TAMANHO(3) + MSG(999) + ETB(1) + CRC(2)] = 1009 bytes

   // Se msg > 0 entao adicionar 0's a esquerda para completar tamanho
   // Pode haver CMDs sem mensagem, como ex: PP_StartGetKey
   if(msg.Length() > 0){
      // Enviara apenas os 999 primeiros bytes e descarta o restante
      msg = msg.SubString(1,999);
      strLenMsg = PADL(String(msg.Length()), 3, '0');
      lenMsg = msg.Length();
   }

   int rslt = 0;

   try{
      // checa conexao serial
      r = CheckComm();

      // Conexao pinpad OK, montar buffer envio
      if(r == PP_OK){
         // Inicio mensagem SYN (16H) -> NAO CONTAR NO CRC! <-
         bufferTX[len++] = SYN;

         // DADOS_MSG (tamanho max 1024)
         // cmd
         memcpy(&bufferTX[len], cmd.c_str(), 3);
         len+=3;

         // Havendo msg, colocar em DADOS_MSG
         if(msg.Length() > 0){
            // tamanho mensagem
            memcpy(&bufferTX[len], strLenMsg.c_str(), 3);
            len+=3;
            // mensagem
            memcpy(&bufferTX[len], msg.c_str(), lenMsg);
            len+=lenMsg;
         }

         // Fim mensagem ETB (17H)
         bufferTX[len++] = ETB;

         // crc =  crc(DADOS_MSG + ETB), nao contar "SYN"
         crc = CRC_Calc(&bufferTX[1], len-1);
         crc = swapWord(crc);
         memcpy(&bufferTX[len], &crc, 2);
         len+=2;

         // Tenta envio/recebimento buffer por 3x, timeout 2s
         for(int i=0; i<3; i++){
            Comm->Write(bufferTX, len);

            r = RcvComm(bufferRX, &len_rx);

            // Recebeu ACK? Termine de receber msg
            if(r == ACK_RCV){
               if(receberApenasACK == true){
                  rslt = r;
                  break;
               }
               r = RcvComm(bufferRX, &len_rx);
               // Recebeu RX_BUFFER_OK? OK, recebeu msg
               if(r == RX_BUFFER_OK){
                  // retorna pelo ponteiro output a mensagem recebida do pinpad
                  // sem o primeiro byte SYN, ETB e CRC
                  *output = BufferToString(&bufferRX[1], len_rx-3);
                  // rslt recebe resultado da operacao: ok, nok, invalido e etc
                  // se falhar conversao, resultado = ERROR_ABECS
                  rslt = BufferToString(&bufferRX[4], 3).ToIntDef(ERROR_ABECS);
                  break;
               }
               // CRC do bufferRX do pinpad n bateu? Enviar NAK
               else if(r == RX_CRC_NOK){
                  Comm->Putc(NAK);
               }
               // Timeout?
               else if(r == RX_BUFFER_TOUT){
                  rslt = r;
               }
            }
            // Recebeu NAK? RX_BUFFER_INV?
            else{
               // bufferTX sera reenviado pelo loop. Aguardar proxima msg
               rslt = r;
            }
         }
      }
      // Conexao pinpad nao OK
      else{
         rslt = r;
      }
   }
   catch(Exception &e){
      rslt = ERROR_ABECS;
   }

   return rslt;
}
//---------------------------------------------------------------------------
int __fastcall TThrSerialCom::RcvComm(unsigned char *buffer, int *len_rx, unsigned long msTimeout)
{
   int byte_recebido = 0;
   int idx = 0;
   unsigned long tempo = MS_TIMER;
   bool rcvETB = false;
   int posETB = 0;
   unsigned short crc = 0; // calculo crc buffer recebido
   unsigned short rcv_crc = 0; // crc recebido no buffer

   // Zerar valores de buffer e len_rx
   memset(buffer, 0, sizeof(buffer));
   *len_rx = 0;

   while(true){
      byte_recebido = Comm->Getc();

      // Se byte_recebido = -1, nao ha bytes para leitura
      if(byte_recebido != -1){
         // Recebeu NAK, msg nao aceita
         if(idx == 0 && byte_recebido == NAK){
            return NAK_RCV;
         }
         // Recebeu EOT, provavel resposta de PP_ABORT()
         else if(idx == 0 && byte_recebido == EOT){
            return EOT_RCV;
         }
         // Recebeu ACK, msg aceita
         else if(idx == 0 && byte_recebido == ACK){
            return ACK_RCV;
         }
         // Recebeu byte inicio msg "SYN"?
         else if(idx == 0 && byte_recebido == SYN) {
            buffer[idx]=(unsigned char)byte_recebido;
            idx++;
         }
         // Nao sendo SYN o primeiro byte, byte invalido
         else if(idx == 0){
            //"Byte recebido INVALIDO " + IntToHex(byte_recebido,2);
            return RX_BUFFER_INV;
         }
         // Receber os demais bytes
         else{
            buffer[idx] = (unsigned char)byte_recebido;
            // Recebeu byte fim mensagem "ETB"? -> Marcar como recebido ETB.
            // Isso é necessario caso o PP envie 2 mensagens concatenadas,
            // neste caso iremos aceitar apenas a primeira
            if(buffer[idx] == ETB && rcvETB == false){
               rcvETB = true;
               posETB = idx;
            }
            idx++;
         }
      }
      // sendo byte_recebido = -1, ou seja nao ha mais anda a receber
      // Se idx for maior que posETB + 2 bytes CRC e o buffer contenha ETB
      else if(idx >= posETB+2 && rcvETB == true){

         // Calcular CRC pelo buffer recebido, crc = crc(DADOS_MSG + ETB)
         crc = CRC_Calc(&buffer[1], posETB);
         crc = swapWord(crc);

         // CRC recebido na msg do pinpad
         memcpy(&rcv_crc, &buffer[posETB+1], 2);

         // CRC calculado é diferente do CRC recebido?
         if(crc != rcv_crc){
            return RX_CRC_NOK;
         }

         // No retorno de RcvComm volta int como status + ponteiro de bufferRX e tamanho bufferRX
         // tamanho bufferRX conta o CRC (posETB+2)
         *len_rx = posETB+2;
         return RX_BUFFER_OK;
      }

      // Se nao recebeu nenhum byte e excedeu tempo timeout
      if (idx == 0 && MS_TIMER-tempo > msTimeout){
         return RX_BUFFER_TOUT;
      }
   }
}
//---------------------------------------------------------------------------
