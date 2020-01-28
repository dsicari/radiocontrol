//---------------------------------------------------------------------------

#ifndef UThrComSerialH
#define UThrComSerialH
//---------------------------------------------------------------------------
#include <Classes.hpp>

#include "UComSerial.h"

#define TIMEOUT_RX      1000
#define TAM_MAX_BUFFER  64
#define RETRIES         3

#define COM_OK          0
#define RX_BUFFER_OK    0
#define COM_NOT_OPEN   -1
#define COM_ERROR      -2
#define RX_BUFFER_TOUT -3

#ifndef MS_TIMER
   #define MS_TIMER ((unsigned long)GetTickCount())
#endif

//---------------------------------------------------------------------------
class TThrComSerial : public TThread
{
   public:
      enum TSTThread {stIdle = 0, stRunning = 1};

   private:
      String __fastcall BufferToString(const void *_buffer, int len, bool forceHex = false);
      int __fastcall CheckComm();
      int __fastcall RcvComm(unsigned char *buffer, int *len_rx, unsigned long msTimeout = TIMEOUT_RX);
      int __fastcall SendRcv(String input, String *output);
      bool Enviar;
      bool CancelaRcv;
      String BufferTX;
      String ComPort;
      int BaudRate;

      TSTThread FSTThread;
      void __fastcall SetSTThread(TSTThread value);

   protected:
      void __fastcall Execute();
      TComSerial* Comm;

   public:
      String BufferRX;
      String LastError;
      String LastMessage;

      __fastcall TThrComSerial(bool CreateSuspended, String comPort, int baudrate);
      __fastcall ~TThrComSerial();

      bool __fastcall OpenSerialPort(String comPort, int baudRate);
      bool __fastcall CloseSerialPort();
      void __fastcall Send(String input);

      __property TSTThread STThread = {read = FSTThread, write = SetSTThread};

      // Eventos gerados
      TThreadMethod onAtualizacaoDados;
};
//---------------------------------------------------------------------------
#endif
