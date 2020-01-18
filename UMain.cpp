//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    Memo1->Clear();
    Ini=new TIniFile(ExtractFilePath(Application->ExeName) + "\RadioControl.ini");
    if(Ini==NULL)("Ini fail");

    if(ThrComSerial==NULL){
        ThrComSerial = new TThrComSerial(true, "COM3", 115200);
        ThrComSerial->onAtualizacaoDados = AtualizacaoDados;
        ThrComSerial->Resume();
        Log("Conectado");  
    }
    else{
        Log("ThrComSerial is not null");
    }
}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
    if(Ini != NULL) delete Ini;
}
//---------------------------------------------------------------------------
__fastcall void TForm1::Log(String str)
{
    Memo1->Lines->Add(DateTimeToStr(Now()) + " " + str);
}
//---------------------------------------------------------------------------
__fastcall void TForm1::AtualizacaoDados()
{
    // ...
}
//---------------------------------------------------------------------------
