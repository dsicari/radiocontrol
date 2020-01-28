//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
    Memo1->Clear();
    btnControlDTR->Caption="TURN ON";
    lblStatusDeviceDTR->Caption="OFF";
    btnControlRTS->Caption="TURN ON";
    lblStatusDeviceRTS->Caption="OFF";

    Ini=NULL;
    ComSerial=NULL;
    ThrComSerial=NULL;

    Ini=new TIniFile(ExtractFilePath(Application->ExeName) + "\RadioControl.ini");
    if(Ini==NULL){
        Log("Ini fail");
    }
    else{
        // Last Comport configuration
        editComPortDevice->Text = Ini->ReadString("COMDEVICE", "PORT", "");
        editComBaudrateDevice->Text = Ini->ReadString("COMDEVICE", "BAUDRATE", "");
        editComPortCAT->Text = Ini->ReadString("COMCAT", "PORT", "");
        editComBaudrateCAT->Text = Ini->ReadString("COMCAT", "BAUDRATE", "");
        // Device configuration
        lblControlDeviceDTR->Caption = Ini->ReadString("DEVICE", "ONDTR", "DEVICE 1");
        lblControlDeviceRTS->Caption = Ini->ReadString("DEVICE", "ONRTS", "DEVICE 2");
    }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    if(Ini != NULL) delete Ini;

    if(ComSerial != NULL){
        if(ComSerial->Enabled())
            ComSerial->Close();
        delete ComSerial;
    }
}
//---------------------------------------------------------------------------
void TMainForm::Log(String str)
{
    Memo1->Lines->Add(DateTimeToStr(Now()) + " " + str);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnOpenComPortDeviceClick(TObject *Sender)
{
   if(ComSerial!=NULL && ComSerial->Enabled()){
      ComSerial->Close();
      delete ComSerial;
      ComSerial=NULL;
      btnOpenComPortDevice->Caption="Open";
      Log("Success closing comport");
      return;
   }

     try{
        if(ComSerial!=NULL){
            delete ComSerial;
            ComSerial=NULL;
        }
        ComSerial = new TComSerial(ComSerial, editComPortDevice->Text, editComBaudrateDevice->Text.ToInt());
        ComSerial->DTROFF();
        ComSerial->RTSOFF();
        ComSerial->Open();
        if(ComSerial->Enabled()){
            Ini->WriteString("COMDEVICE", "PORT", editComPortDevice->Text);
            Ini->WriteString("COMDEVICE", "BAUDRATE", editComBaudrateDevice->Text);
            btnOpenComPortDevice->Caption = "Close";
            btnControlDTR->Caption="TURN ON";
            lblStatusDeviceDTR->Caption="OFF";
            btnControlRTS->Caption="TURN ON";
            lblStatusDeviceRTS->Caption="OFF";
            Log("Success opening comport=" + editComPortDevice->Text + ", baudrate=" + editComBaudrateDevice->Text);
        }
        else{
            Log("Can not open comport=" + editComPortDevice->Text + ", baudrate=" + editComBaudrateDevice->Text);
        }
    }
    catch(...)
    {
        Log("Fail open comport=" + editComPortDevice->Text + ", baudrate=" + editComBaudrateDevice->Text);
        if(ComSerial != NULL){
            delete ComSerial;
            ComSerial=NULL;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnControlDTRClick(TObject *Sender)
{
    if(ComSerial!=NULL && ComSerial->Enabled()){
        if(btnControlDTR->Caption == "TURN OFF"){
            ComSerial->DTROFF();
            Log("DTR OFF");
            btnControlDTR->Caption="TURN ON";
            lblStatusDeviceDTR->Caption="OFF";
        }
        else{
            ComSerial->DTRON();
            Log("DTR ON");
            btnControlDTR->Caption="TURN OFF";
            lblStatusDeviceDTR->Caption="ON";
        }
    }
    else{
        Log("Comport not open");
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnControlRTSClick(TObject *Sender)
{
    if(ComSerial!=NULL && ComSerial->Enabled()){
        if(btnControlRTS->Caption == "TURN OFF"){
            ComSerial->RTSOFF();
            Log("RTS OFF");
            btnControlRTS->Caption="TURN ON";
            lblStatusDeviceRTS->Caption="OFF";
        }
        else{
            ComSerial->RTSON();
            Log("RTS ON");
            btnControlRTS->Caption="TURN OFF";
            lblStatusDeviceRTS->Caption="ON";
        }
    }
    else{
        Log("Comport not open");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnOpenComPortCATClick(TObject *Sender)
{
   if(ThrComSerial==NULL){
      ThrComSerial = new TThrComSerial(true, editComPortCAT->Text, editComBaudrateCAT->Text.ToIntDef(38400));
      ThrComSerial->onAtualizacaoDados = AtualizacaoDados;
      ThrComSerial->Resume();
      Log("ThrComSerial OK");
      btnOpenComPortCAT->Caption="Close";
    }
    else{
      ThrComSerial->Terminate();
      ThrComSerial->WaitFor();
      delete ThrComSerial;
      ThrComSerial = NULL;
      btnOpenComPortCAT->Caption="Open";
      Log("ThrComSerial Closed");
    }    
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AtualizacaoDados()
{
   if(ThrComSerial != NULL)
   {
      if(ThrComSerial->LastError != "")
         Log("ThrComSerial ERROR: " + ThrComSerial->LastError);
       if(ThrComSerial->LastMessage != "")
         Log("ThrComSerial Message: " + ThrComSerial->LastMessage);
      if(ThrComSerial->BufferRX != "")
         Log("ThrComSerial Received: " + ThrComSerial->BufferRX);
   }
}
void __fastcall TMainForm::btnCATCmdFAClick(TObject *Sender)
{
   if(ThrComSerial != NULL){
      ThrComSerial->Send("FA;");
   }
}
//---------------------------------------------------------------------------

