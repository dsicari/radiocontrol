//---------------------------------------------------------------------------

#ifndef UThrComSerialH
#define UThrComSerialH
//---------------------------------------------------------------------------
#include <Classes.hpp>

#include "UComSerial.h"

//---------------------------------------------------------------------------
class TThrSerialCom : public TThread
{
   public:
      enum TSTThread {stIdle = 0, stRunning = 1};

   private:
      UINT16 __fastcall CRC_Calc(unsigned char *pbData, int iLength);
      String __fastcall BufferToString(const void *_buffer, int len, bool forceHex = false);
      int __fastcall CheckComm();
      int __fastcall SendAbort();
      int __fastcall RcvComm(unsigned char *buffer, int *len_rx, unsigned long msTimeout = TIMEOUT_RX);
      bool EnviarCmd;
      bool CancelaRcv;
      String Msg2Send;

      String FCmd2Send;
      void __fastcall SetCmd2Send(String value);

      TSTThread FSTThread;
      void __fastcall SetSTThread(TSTThread value);

   protected:
      void __fastcall Execute();
      TComSerial* Comm;

   public:
      String StrBufferRX;

      __fastcall TThrSerialCom(bool CreateSuspended);
      __fastcall ~TThrSerialCom();

      int __fastcall OpenSerialPort(String comPort, int baudRate);
      int __fastcall CloseSerialPort();
      int __fastcall Send(String cmd, String msg, bool retornarUltimaFBlocante = false);
      int __fastcall SendRcv(String cmd, String msg, String *output, bool receberApenasACK = false);

      __property TSTThread STThread = {read = FSTThread, write = SetSTThread};
      __property String Cmd2Send = {read = FCmd2Send, write = SetCmd2Send};

      // Eventos gerados
      TThreadMethod onAtualizacaoDados;
};
//---------------------------------------------------------------------------
#endif
