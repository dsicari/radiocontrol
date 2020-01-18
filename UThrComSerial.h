//---------------------------------------------------------------------------

#ifndef threadComH
#define threadComH
//---------------------------------------------------------------------------
#include <Classes.hpp>

#include "UComSerial.h"

//---------------------------------------------------------------------------
class TThrComSerial : public TThread
{            
private:
   bool doSendAscii;
   String SendBuffer;
   TComSerial* Com;

protected:
   void __fastcall Execute();

public:
   __fastcall TThrComSerial(bool CreateSuspended, String comPort, String baudrate);
   __fastcall ~TThrComSerial();
   void __fastcall sendAscii(String msg);

   String ReceiveBuffer;

   TThreadMethod onAtualizacaoDados;
};

//---------------------------------------------------------------------------
#endif
