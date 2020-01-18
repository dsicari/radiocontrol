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
}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
    if(Ini != NULL) delete Ini;
}
//---------------------------------------------------------------------------
void Log(String str)
{
    Memo1->Lines->Add(Now() + " " + str);
}
//---------------------------------------------------------------------------
