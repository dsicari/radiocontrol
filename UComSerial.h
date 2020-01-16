//---------------------------------------------------------------------------

#ifndef UComSerialH
#define UComSerialH

//---------------------------------------------------------------------------
#include <Classes.hpp>


//---------------------------------------------------------------------------
class TComSerial : public TComponent
{
__published:	// IDE-managed Components

private:	// User declarations
   AnsiString device;
   AnsiString deviceNomePorta;
   DWORD BaudRate;
   DCB dcb_save;
   DCB dcb;
   HANDLE handle;
   bool ativo;

public:	// User declarations
   __fastcall TComSerial(TComponent* Owner);
   __fastcall TComSerial(TComponent* Owner, const AnsiString& SerialDevice, DWORD BaudRate);
   __fastcall ~TComSerial(void);
   bool __fastcall Enabled(void); // para saver se a porta esta aberta
   void __fastcall Open(void);
   void __fastcall Close(void);
   void __fastcall RTSON(void);
   void __fastcall RTSOFF(void);
   void __fastcall DTRON(void);
   void __fastcall DTROFF(void);
   void __fastcall SETBAUD(DWORD BaudRate);
   DWORD __fastcall GETBAUD(void);
   void __fastcall SETDEVICE(const AnsiString& SerialDevice);
   int __fastcall Write(void*,int); // retorna: 0 para sucesso, -1 erro
   int __fastcall Putc(char); // retorna: 0 para sucesso, -1 erro
   int __fastcall Getc(void); // retorna: -1 se nao ha caracteres senao o caracter
};
//---------------------------------------------------------------------------

/************************************************
*Valores padroes BaudRate:
*         CBR_110 	      CBR_19200
*         CBR_300 	      CBR_38400
*         CBR_600 	      CBR_56000
*         CBR_1200	      CBR_57600
*         CBR_2400	      CBR_115200
*         CBR_4800	      CBR_128000
*         CBR_9600	      CBR_256000
*         CBR_14400
* as defines sao igrais a valor exemplo CBR_115200 = 115200
*  para portas USB pode usar o valor 230400
************************************************/

//---------------------------------------------------------------------------
#endif