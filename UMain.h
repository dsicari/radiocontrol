//---------------------------------------------------------------------------

#ifndef UMainH
#define UMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//inifile
#include <inifiles.hpp>
//threadCom
#include "UThrComSerial.h"

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TMemo *Memo1;
private:	// User declarations
public:		// User declarations
    TIniFile *Ini;
    TThrComSerial *ThrComSerial;
    __fastcall TForm1(TComponent* Owner);
    __fastcall ~TForm1();
    __fastcall void Log(String str);
    __fastcall void AtualizacaoDados();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
