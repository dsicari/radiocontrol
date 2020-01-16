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
protected:
   void __fastcall Execute();
public:
   __fastcall TThrComSerial(bool CreateSuspended, String comPort, String baudrate);
   __fastcall ~TThrComSerial();
   void __fastcall trataDados();
   int __fastcall sendRecv(unsigned char cmd, const void* dados, int length, void* dados_rec, int dados_rec_len_max, int* dados_rec_len);
   int __fastcall monta_pacote(TAtmMonResult* pkt, unsigned char cmd, unsigned char sequencia);
   void __fastcall firmware();
   void __fastcall reset();
   void __fastcall rele();
   TThreadMethod onAtualizacaoDados;
   TThreadMethod onAtualizaStatus;
   TThreadMethod onAtualizaMemoCom;
   TThreadMethod onRequestFirmware;
   TComSerial* Com;
};

//---------------------------------------------------------------------------
#endif
