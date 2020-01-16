//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>

#include "UComSerial.h"


//--------------------------------------------------------------------------------
__fastcall TComSerial::TComSerial(TComponent* Owner)
    : TComponent(Owner)
{
   device="COM1";
   BaudRate=CBR_9600;
   ativo=false;
}
//--------------------------------------------------------------------------------
__fastcall TComSerial::TComSerial(TComponent* Owner, const AnsiString& SerialDevice, DWORD __BaudRate)
    : TComponent(Owner)
{
   BaudRate=__BaudRate;
   device=SerialDevice;
   if ( device.Pos("\\\\.\\") == 1 )
      deviceNomePorta=device.SubString(5,device.Length()-4);
   else
      deviceNomePorta=device;
   ativo=false;
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::SETDEVICE(const AnsiString& SerialDevice)
{
   if ( device == SerialDevice )
      return;
   device=SerialDevice;
   if ( device.Pos("\\\\.\\") == 1 )
      deviceNomePorta=device.SubString(5,device.Length()-4);
   else
      deviceNomePorta=device;
   if ( ativo ) {
      Close();
      Open();
   }
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::Open()
{
   COMMTIMEOUTS CommTimeOuts;

   Close();

   handle=CreateFile(device.c_str(), GENERIC_READ|GENERIC_WRITE, 0,
                                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

   if ( handle == INVALID_HANDLE_VALUE ) {
      throw Exception("Erro open Device (" + deviceNomePorta + ")");
   }

   if ( !GetCommState(handle, &dcb) ) {
      CloseHandle(handle);
      throw Exception("Erro GetCommState Device (" + deviceNomePorta + ")");
   }

   memcpy(&dcb_save,&dcb,sizeof(DCB));

//   COMMPROP CommProperts;
//   if ( !GetCommProperties(handle,&CommProperts) ) {
//      CloseHandle(handle);
//      throw Exception("Erro GetCommProperties Device (" + deviceNomePorta + ")");
//   }
//   Log((int)CommProperts.dwMaxRxQueue);
//   Log((int)CommProperts.dwMaxTxQueue);
//   Log((int)CommProperts.wPacketLength);
//   Log((int)CommProperts.dwCurrentRxQueue);

   CommTimeOuts.ReadIntervalTimeout=MAXDWORD;
   CommTimeOuts.ReadTotalTimeoutMultiplier=0;
   CommTimeOuts.ReadTotalTimeoutConstant=0;
   CommTimeOuts.WriteTotalTimeoutMultiplier=0;
   CommTimeOuts.WriteTotalTimeoutConstant=0;

   if ( SetCommTimeouts(handle, &CommTimeOuts) == 0 ) {
      CloseHandle(handle);
      throw Exception("Erro SetCommTimeouts Device (" + deviceNomePorta + ")");
   }

   dcb.BaudRate=BaudRate;
   dcb.fOutxCtsFlow=false;
   dcb.fOutxDsrFlow=false;
   dcb.fDtrControl=DTR_CONTROL_DISABLE;
   //DANILO: para manter DTR sem pulsar depois da primeira conexao
   dcb_save.fDtrControl=DTR_CONTROL_DISABLE;
   dcb.fDsrSensitivity=false;
   dcb.fOutX=false;
   dcb.fInX=false;
   dcb.fNull=false;
   dcb.fRtsControl=RTS_CONTROL_DISABLE;  // RTS_CONTROL_DISABLE	// RTS_CONTROL_ENABLE
   dcb.fAbortOnError=false;
   dcb.ByteSize=8;
   dcb.fParity=false;
   dcb.Parity=NOPARITY;

   if ( !SetCommState(handle, &dcb) ) {
      //EscapeCommFunction(handle, SETDTR);  // danilo
      SetCommState(handle, &dcb_save);
      CloseHandle(handle);
      throw Exception("[Open]Erro SetCommState Device (" + deviceNomePorta + ")");
   }

//   if ( modo == MODO_FOPEN ) {
//      CloseHandle(handle);
//      if ( (fhandle=fopen(device.c_str(),"wb+")) == NULL ) {
//         throw Exception("Erro open Device (" + device + ")");
//      }
//   }

   ativo=true;

}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::RTSON()
{
   if ( !ativo )
      return;
   dcb.fRtsControl=RTS_CONTROL_ENABLE;
   if ( !SetCommState(handle, &dcb) ) {
      SetCommState(handle, &dcb_save);
      CloseHandle(handle);
      throw Exception("[RTSON]Erro SetCommState Device (" + deviceNomePorta + ")");
   }
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::RTSOFF()
{
   if ( !ativo )
      return;
   dcb.fRtsControl=RTS_CONTROL_DISABLE;
   if ( !SetCommState(handle, &dcb) ) {
      SetCommState(handle, &dcb_save);
      CloseHandle(handle);
      throw Exception("[RTSOFF]Erro SetCommState Device (" + deviceNomePorta + ")");
   }
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::DTRON()
{
   if ( !ativo )
      return;
   dcb.fDtrControl=DTR_CONTROL_ENABLE;
   if ( !SetCommState(handle, &dcb) ) {
      SetCommState(handle, &dcb_save);
      CloseHandle(handle);
      throw Exception("[DTRON]Erro SetCommState Device (" + deviceNomePorta + ")");
   }
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::DTROFF()
{
   if ( !ativo )
      return;
   dcb.fDtrControl=DTR_CONTROL_DISABLE;
   if ( !SetCommState(handle, &dcb) ) {
      SetCommState(handle, &dcb_save);
      CloseHandle(handle);
      throw Exception("[DTROFF]Erro SetCommState Device (" + deviceNomePorta + ")");
   }
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::SETBAUD(DWORD __BaudRate)
{
   BaudRate=__BaudRate;
   if ( !ativo )
      return;
   dcb.BaudRate=BaudRate;
   if ( !SetCommState(handle, &dcb) ) {
      SetCommState(handle, &dcb_save);
      CloseHandle(handle);
      throw Exception("[SETBAUD]Erro SetCommState Device (" + deviceNomePorta + ")");
   }
}
//--------------------------------------------------------------------------------
DWORD __fastcall TComSerial::GETBAUD(void)
{
   return BaudRate;
}
//--------------------------------------------------------------------------------
void __fastcall TComSerial::Close()
{
   if ( ativo ) {
      SetCommState(handle,&dcb_save);
      CloseHandle(handle);
      ativo=false;
   }
}
//--------------------------------------------------------------------------------
int __fastcall TComSerial::Write(void* buffer, int enviar)
{
   unsigned long int enviado;
   if ( !ativo )
      throw Exception("[TComSerial::Write] Serial Closed");
   enviado=enviar;
   if ( WriteFile(handle, buffer, enviar, &enviado, NULL) == 0 )
      return -1;
   if ( (int)enviado != enviar )
      return -1;
   return 0;
}
//--------------------------------------------------------------------------------
int __fastcall TComSerial::Putc(char c)
{
   if ( !ativo )
      throw Exception("[TComSerial::Putc] Serial Closed");
   return Write(&c, 1);
}
//--------------------------------------------------------------------------------
int __fastcall TComSerial::Getc()
{
   char* buffer[16];
   unsigned long int recebido;
   int i,rslt;

   if ( !ativo )
      throw Exception("[TComSerial::Getc] Serial Closed");

   recebido=0;
   i=ReadFile(handle, buffer, 1, &recebido, NULL);
   if ( i == 0 ) {
      //Log("rx erro");
      rslt=-1;
   }
   else if ( recebido > 0 ) {
      rslt=(int)(unsigned char)buffer[0];
   }
   else {
      rslt=-1;
   }
   return rslt;
}
//--------------------------------------------------------------------------------
bool __fastcall TComSerial::Enabled()
{
   return ativo;
}
//--------------------------------------------------------------------------------
__fastcall TComSerial::~TComSerial()
{
   Close();
}
//--------------------------------------------------------------------------------

