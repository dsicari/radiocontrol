//---------------------------------------------------------------------------

#ifndef UMainH
#define UMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
// inifile
#include <inifiles.hpp>
// Com for device ON/OFF
#include "UComSerial.h"
// threadCom for CAT
#include "UThrComSerial.h"
//---------------------------------------------------------------------------
class TmainForm : public TForm
{
__published:	// IDE-managed Components
    TMemo *Memo1;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TEdit *editComPortCAT;
    TLabel *Label1;
    TEdit *editComBaudrateCAT;
    TLabel *Label2;
    TButton *btnOpenComPortCAT;
    TGroupBox *GroupBox3;
    TLabel *Label3;
    TLabel *Label4;
    TEdit *editComPortDevice;
    TEdit *editComBaudrateDevice;
    TButton *btnOpenComPortDevice;
    TGroupBox *gboxControlDevice;
    TButton *btnControlDTR;
    TButton *btnControlRTS;
    TLabel *lblControlDeviceDTR;
    TLabel *lblControlDeviceRTS;
    TLabel *lblStatusDeviceDTR;
    TLabel *lblStatusDeviceRTS;
    void __fastcall btnOpenComPortDeviceClick(TObject *Sender);
    void __fastcall btnControlDTRClick(TObject *Sender);
    void __fastcall btnControlRTSClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    TIniFile *Ini;
    TComSerial *ComSerial;
    TThrComSerial *ThrComSerial;
    __fastcall TmainForm(TComponent* Owner);
    __fastcall ~TmainForm();
    void Log(String str);
    void AtualizacaoDados();
};
//---------------------------------------------------------------------------
extern PACKAGE TmainForm *mainForm;
//---------------------------------------------------------------------------
#endif
