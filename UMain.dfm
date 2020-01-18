object mainForm: TmainForm
  Left = 609
  Top = 302
  Width = 732
  Height = 402
  Caption = 'Radio Control'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 168
    Width = 724
    Height = 207
    Align = alBottom
    Lines.Strings = (
      'Memo1')
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 337
    Height = 145
    Caption = 'Serial Port'
    TabOrder = 1
    object GroupBox2: TGroupBox
      Left = 176
      Top = 24
      Width = 145
      Height = 105
      Caption = 'CAT'
      TabOrder = 0
      object Label1: TLabel
        Left = 10
        Top = 16
        Width = 19
        Height = 13
        Caption = 'Port'
      end
      object Label2: TLabel
        Left = 78
        Top = 16
        Width = 43
        Height = 13
        Caption = 'Baudrate'
      end
      object editComPortCAT: TEdit
        Left = 10
        Top = 32
        Width = 57
        Height = 21
        TabOrder = 0
      end
      object editComBaudrateCAT: TEdit
        Left = 78
        Top = 32
        Width = 57
        Height = 21
        TabOrder = 1
      end
      object btnOpenComPortCAT: TButton
        Left = 32
        Top = 64
        Width = 75
        Height = 25
        Caption = 'Open'
        TabOrder = 2
      end
    end
    object GroupBox3: TGroupBox
      Left = 16
      Top = 24
      Width = 145
      Height = 105
      Caption = 'Device'
      TabOrder = 1
      object Label3: TLabel
        Left = 10
        Top = 16
        Width = 19
        Height = 13
        Caption = 'Port'
      end
      object Label4: TLabel
        Left = 78
        Top = 16
        Width = 43
        Height = 13
        Caption = 'Baudrate'
      end
      object editComPortDevice: TEdit
        Left = 10
        Top = 32
        Width = 57
        Height = 21
        TabOrder = 0
      end
      object editComBaudrateDevice: TEdit
        Left = 78
        Top = 32
        Width = 57
        Height = 21
        TabOrder = 1
      end
      object btnOpenComPortDevice: TButton
        Left = 32
        Top = 64
        Width = 75
        Height = 25
        Caption = 'Open'
        TabOrder = 2
        OnClick = btnOpenComPortDeviceClick
      end
    end
  end
  object gboxControlDevice: TGroupBox
    Left = 360
    Top = 8
    Width = 217
    Height = 145
    Caption = 'Control'
    TabOrder = 2
    object lblControlDeviceDTR: TLabel
      Left = 104
      Top = 24
      Width = 100
      Height = 13
      Alignment = taCenter
      Caption = 'lblControlDeviceDTR'
      Layout = tlCenter
    end
    object lblControlDeviceRTS: TLabel
      Left = 104
      Top = 88
      Width = 99
      Height = 13
      Alignment = taCenter
      Caption = 'lblControlDeviceRTS'
      Layout = tlCenter
    end
    object lblStatusDeviceDTR: TLabel
      Left = 104
      Top = 48
      Width = 97
      Height = 13
      Caption = 'lblStatusDeviceDTR'
    end
    object lblStatusDeviceRTS: TLabel
      Left = 104
      Top = 112
      Width = 96
      Height = 13
      Caption = 'lblStatusDeviceRTS'
    end
    object btnControlDTR: TButton
      Left = 16
      Top = 40
      Width = 75
      Height = 25
      Caption = 'btnControlDTR'
      TabOrder = 0
      OnClick = btnControlDTRClick
    end
    object btnControlRTS: TButton
      Left = 16
      Top = 104
      Width = 75
      Height = 25
      Caption = 'btnControlRTS'
      TabOrder = 1
      OnClick = btnControlRTSClick
    end
  end
end
