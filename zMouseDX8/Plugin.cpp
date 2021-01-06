
// This file added in headers queue
// File: "Sources.h"

namespace NAMESPACE {

  static LPDIRECTINPUT8        m_lpDInput      = Null;
  LPDIRECTINPUTDEVICE8A        m_lpDIMouse     = Null;
  static LPDIRECTINPUTDEVICE8A m_lpDIJoystick  = Null;
  static const int             m_nBufferSize   = 16;

  bool*              keyevent   =  (bool*)        ZenDef( 0x0086CCC8, 0x008B27C0, 0x008C3020, 0x008D1678 );
  bool*              keytoggle  =  (bool*)        ZenDef( 0x0086CED0, 0x008B29D4, 0x008C3260, 0x008D18B8 );
  zCArray<int>&      keybuffer  = *(zCArray<int>*)ZenDef( 0x0086D2DC, 0x008B2E00, 0x008C36F0, 0x008D1D50 );
  zTMouseState&      mouseState = *(zTMouseState*)ZenDef( 0x0086CCAC, 0x008B27A8, 0x008C3004, 0x008D165C );
  TRealMousePosition smoothMouseState;












  // int __cdecl Apply_Options_Video(void)
  int Apply_Options_Video();

  HOOK Ivk_Apply_Options_Video AS( &Apply_Options_Video, &Apply_Options_Video );

  __declspec(noinline) int Apply_Options_Video() {
    if( zinput && zinput->GetDeviceEnabled( zINPUT_MOUSE ) )
      ShowCursor( False );

    return Ivk_Apply_Options_Video();
  }








  float TRealMousePosition::Length() {
    return sqrt( x * x + y * y );
  }

  void TRealMousePosition::SetX( const float& v ) {
    x = v;
    mouseState.xpos = (int)ceil( x );
  }

  void TRealMousePosition::SetY( const float& v ) {
    y = v;
    mouseState.xpos = (int)ceil( y );
  }

  void TRealMousePosition::SetZ( const float& v ) {
    z = v;
    mouseState.zpos = (int)ceil( z );
  }

  float TRealMousePosition::GetX() {
    return x * SpeedMultX;
  }

  float TRealMousePosition::GetY() {
    return y * SpeedMultY;
  }

  float TRealMousePosition::GetZ() {
    return z;
  }

  void TRealMousePosition::Reset() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
  }

  void TRealMousePosition::Smooth() {
    if( PrecisionEnabled ) {
      float length = Length();
      if( length < PrecisionMult ) {
        float smoothMult = sin( length / PrecisionMult );
        x *= smoothMult;
        y *= smoothMult;
      }
    }
  }

  void TRealMousePosition::OnInit() {
    Union.GetUnionOption().Read( SpeedMultX,    "MOUSE", "SpeedMultiplierX",    1.0f );
    Union.GetUnionOption().Read( SpeedMultY,    "MOUSE", "SpeedMultiplierY",    1.0f );
    Union.GetUnionOption().Read( PrecisionMult, "MOUSE", "PrecisionMultiplier", 1.0f );
    if( SpeedMultX == 0.0f )
      SpeedMultX = 1.0f;

    if( SpeedMultY == 0.0f )
      SpeedMultY = 1.0f;

    if( PrecisionMult > 0.0f )
      PrecisionEnabled = True;
  }


   





  

  
  // Создает девайс мышки
  typedef void* ( *LPGETHANDLE )( void );

  bool CreateMouseDevice() {
    if( !m_lpDInput ) {
      HINSTANCE hInst = ::GetModuleHandle( Null );
      HRESULT hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&m_lpDInput, Null );
      if( FAILED( hr ) ) {
        Message::Error( "#1: Direct input does not create. Return original methods." );
        return false;
      }
    }
    if( !m_lpDIMouse ) {
      if( m_lpDInput->CreateDevice( GUID_SysMouse, &m_lpDIMouse, Null ) == DI_OK ) {
        if( m_lpDIMouse->SetDataFormat( &c_dfDIMouse2 ) != DI_OK ) {
          Message::Error( "#2: Input device does not create. Return original methods." );
          return false;
        }

        DIPROPDWORD prop;
        prop.diph.dwSize = sizeof( DIPROPDWORD );
        prop.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        prop.diph.dwObj = 0;
        prop.diph.dwHow = DIPH_DEVICE;
        prop.dwData = m_nBufferSize;
        if( m_lpDIMouse->SetProperty( DIPROP_BUFFERSIZE, &prop.diph ) != DI_OK ) {
          Message::Error( "#3: Input priority is not applied. Return original methods." );
          return false;
        }

        prop.diph.dwSize = sizeof( DIPROPDWORD );
        prop.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        prop.diph.dwObj = 0;
        prop.diph.dwHow = DIPH_DEVICE;
        prop.dwData = DIPROPAXISMODE_REL;
        if( m_lpDIMouse->SetProperty( DIPROP_AXISMODE, &prop.diph ) != DI_OK ) {
          Message::Error( "#4: Input properties is not applied. Return original methods." );
          return false;
        }

        HWND hWnd = hWndInput ? hWndInput : hWndApp;
        if( m_lpDIMouse->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) != DI_OK )
          cmd << Col16( CMD_YELLOW ) << "Warning: " << Col16( CMD_YELLOW | CMD_INT ) << "Impossible to set SetCooperativeLevel for mouse" << Col16() << endl;
      }
    }

    smoothMouseState.OnInit();
    return true;
  }




  

  // Вернуть позицию мыши
  static float&  postMultiplierX = *(real32*)ZenDef( 0x00836538, 0x0087C15C, 0x0088BFE0, 0x0089A148 );
  static float&  postMultiplierY = *(real32*)ZenDef( 0x0083653C, 0x0087C160, 0x0088BFE4, 0x0089A14C );
  static bool_t& postInverseX    = *(bool_t*)ZenDef( 0x0086D304, 0x008B2E28, 0x008C3718, 0x008D1D78 );
  static bool_t& postInverseY    = *(bool_t*)ZenDef( 0x0086D308, 0x008B2E2C, 0x008C371C, 0x008D1D7C );

  // HOOK Hook_GetMousePos = AutoModulePatchCallInvoker( Null, &zCInput_Win32::GetMousePos_Union );
  HOOK Hook_GetMousePos PATCH_IF( &zCInput_Win32::GetMousePos, &zCInput_Win32::GetMousePos_Union, false );

  void zCInput_Win32::GetMousePos_Union( float& x, float& y, float& wheel ) {
    x     = smoothMouseState.GetX() * postMultiplierX;
    y     = smoothMouseState.GetY() * postMultiplierY;
    wheel = smoothMouseState.GetZ();
    if( postInverseX ) x = -x;
    if( postInverseY ) y = -y;
  }






  // Установить состояние кнопки
  void SetMouseKeyStateAndInsert( int keyId, DWORD dwData ) {
    bool keyEventBefore = keyevent[keyId];
    keyevent[keyId] = IK_PRESS( dwData );
    if( keyevent[keyId] ) {
      keybuffer.InsertEnd( keyId );
      keytoggle[keyId] = !keyEventBefore;
    }
  }




#define MOUSE_KEY_LEFT_G1  2050
#define MOUSE_KEY_MID_G1   2051
#define MOUSE_KEY_RIGHT_G1 2052
#define MOUSE_WHEELUP_G1   2057
#define MOUSE_WHEELDOWN_G1 2058



  static bool  forceMouseControl  = false;
  static float focusInteruptTimeF = 0.5f;
  static int   focusInteruptTime  = 500;






  int SetAcquireMouse() { return 0; }
  void DisableAcquire() {
#if ENGINE >= Engine_G2
    static HOOK Ivk_SetAcquireMouse AS( &SetAcquireMouse, &SetAcquireMouse );
#else
    static HOOK Ivk_SetAcquire AS( &SetAcquire, &SetAcquireMouse );
#endif
  }




  void UpdateMouse_OnlyAxis();
  void UpdateMouse();



  // Происходит только для вращения
  HOOK Hook_UpdateMouse_OnlyAxis PATCH_IF( UpdateMouse, UpdateMouse_OnlyAxis, false );

  void UpdateMouse_OnlyAxis() {
    static bool newMouseEnabled = true;
           bool mouseEnabled    = zinput->GetDeviceEnabled( zINPUT_MOUSE );

    if( newMouseEnabled && !mouseEnabled ) {
      newMouseEnabled = false;
      m_lpDIMouse->Unacquire();
    }
    else if( !newMouseEnabled && mouseEnabled ) {
      newMouseEnabled = true;
      SetForegroundWindow( GetDesktopWindow() );
      SetForegroundWindow( hWndApp );
    }

    if( !mouseEnabled && !forceMouseControl )
      return;

    m_lpDIMouse->Acquire();
    m_lpDIMouse->Poll();

    smoothMouseState.Reset();

    bool32 done = False;
    for( int i = 0; i < m_nBufferSize; i++ ) {
      DIDEVICEOBJECTDATA data;
      ulong32 elements = 1;
      ulong32 result = m_lpDIMouse->GetDeviceData( sizeof( data ), &data, &elements, 0 );

      if( result == DIERR_INPUTLOST ) {
        m_lpDIMouse->Acquire();
        break;
      }

      if( result == DI_OK && elements == 1 ) {
        switch( data.dwOfs ) {
          case DIMOFS_X:
            smoothMouseState.SetX( (float)(int)data.dwData );
            break;
          case DIMOFS_Y:
            smoothMouseState.SetY( (float)(int)data.dwData );
            break;
          case DIMOFS_Z:
          {
            smoothMouseState.SetZ( (float)(int)data.dwData );
            break;
          }
        }
      }
      else if( elements == 0 ) done = True;
    }

    smoothMouseState.Smooth();
    Hook_UpdateMouse_OnlyAxis();
  }







  // Происходит при полном врапе
  HOOK Hook_UpdateMouse PATCH_IF( UpdateMouse, UpdateMouse, false );

  void UpdateMouse() {
    static bool newMouseEnabled = true;
           bool mouseEnabled    = zinput->GetDeviceEnabled( zINPUT_MOUSE );

    if( newMouseEnabled && !mouseEnabled ) {
      newMouseEnabled = false;
      m_lpDIMouse->Unacquire();
    }
    else if( !newMouseEnabled && mouseEnabled ) {
      newMouseEnabled = true;
      SetForegroundWindow( GetDesktopWindow() );
      SetForegroundWindow( hWndApp );
    }

    if( !mouseEnabled && !forceMouseControl )
      return;

    m_lpDIMouse->Acquire();
    m_lpDIMouse->Poll();

    smoothMouseState.Reset();
    
#if ENGINE != Engine_G1
    SetMouseKeyStateAndInsert( MOUSE_WHEELUP,   False );
    SetMouseKeyStateAndInsert( MOUSE_WHEELDOWN, False );
#endif

    bool32 done = False;
    for( int i = 0; i < m_nBufferSize; i++ ) {
      DIDEVICEOBJECTDATA data;
      ulong32 elements = 1;
      ulong32 result = m_lpDIMouse->GetDeviceData( sizeof( data ), &data, &elements, 0 );

      if( result == DIERR_INPUTLOST ) {
        m_lpDIMouse->Acquire();
        break;
      }

      if( result == DI_OK && elements == 1 ) {
        switch( data.dwOfs ) {
          case DIMOFS_X:
            smoothMouseState.SetX( (float)(int)data.dwData );
            break;
          case DIMOFS_Y:
            smoothMouseState.SetY( (float)(int)data.dwData );
            break;
          case DIMOFS_Z:
          {
            smoothMouseState.SetZ( (float)(int)data.dwData );
            
#if ENGINE != Engine_G1
            int keyId = 0;
            if( smoothMouseState.GetZ() > 0 ) keyId = MOUSE_WHEELUP; else
            if( smoothMouseState.GetZ() < 0 ) keyId = MOUSE_WHEELDOWN;
            if( keyId != 0 )
              SetMouseKeyStateAndInsert( keyId, 0x80 );
#endif
            break;
          }
          case DIMOFS_BUTTON0:
          {
            mouseState.buttonPressedLeft = IK_PRESS( data.dwData );
#if ENGINE != Engine_G1
            SetMouseKeyStateAndInsert( MOUSE_BUTTONLEFT, data.dwData );
#endif
            break;
          }
          case DIMOFS_BUTTON1:
          {
            mouseState.buttonPressedRight = IK_PRESS( data.dwData );
#if ENGINE != Engine_G1
            SetMouseKeyStateAndInsert( MOUSE_BUTTONRIGHT, data.dwData );
#endif
            break;
          }
          case DIMOFS_BUTTON2:
          {
            mouseState.buttonPressedMid = IK_PRESS( data.dwData );
#if ENGINE != Engine_G1
            SetMouseKeyStateAndInsert( MOUSE_BUTTONMID, data.dwData );
#endif
            break;
          }
          
#if ENGINE != Engine_G1
          /* Other buttons */
          case DIMOFS_BUTTON3:
            SetMouseKeyStateAndInsert( MOUSE_XBUTTON1, data.dwData );
            break;
          case DIMOFS_BUTTON4:
            SetMouseKeyStateAndInsert( MOUSE_XBUTTON2, data.dwData );
            break;
          case DIMOFS_BUTTON5:
            SetMouseKeyStateAndInsert( MOUSE_XBUTTON3, data.dwData );
            break;
          case DIMOFS_BUTTON6:
            SetMouseKeyStateAndInsert( MOUSE_XBUTTON4, data.dwData );
            break;
          case DIMOFS_BUTTON7:
            SetMouseKeyStateAndInsert( MOUSE_XBUTTON5, data.dwData );
            break;
#endif
        }
      }
      else if( elements == 0 ) done = True;
    }
    
#if ENGINE == Engine_G1
    zinput->SetState( GAME_ACTION, mouseState.buttonPressedLeft );
    zinput->SetState( GAME_SMOVE,  mouseState.buttonPressedRight );
#endif
    
#if ENGINE >= Engine_G2
    bool_t idle = abs( smoothMouseState.GetX() ) <= 0.0001f ? True : False;
    static Timer idleTimer;
    idleTimer.ClearUnused();

    if( !idle ) {
      idleTimer[0u].Delete();
      ((zCInput_Win32*)zinput)->m_bMouseIdle = idle;
    }
    else if( idleTimer[0u].Await( focusInteruptTime ) )
      ((zCInput_Win32*)zinput)->m_bMouseIdle = idle;
#endif
    
    smoothMouseState.Smooth();
  }








  // Обновляет секцию мыши в ini
  static void ClearOldMouseSection() {
    // Check file version
    if( Union.GetUnionOption().IsExists( "MOUSE", "ApplyFix" ) ) {
      auto& blocks = Union.GetUnionOption().lstBlocks;

      // Get mouse section
      auto list = blocks.GetNext();
      while( list ) {
        auto& block = list->GetData();

        // Remove ini section
        if( block->sBlockName == "MOUSE" ) {
          block->lstValues.DeleteListDatas();
          blocks.Delete( block );

          // Get file data
          auto& doc = Union.GetUnionOption().mDocument;
          for( uint i = 0; i < doc.GetNum(); i++ ) {
            if( (A doc[i]).Shrink() == "[MOUSE]" ) {
              doc.RemoveLinesAt( i );

              // Remove block data
              while( i < doc.GetNum() ) {
                string& line = doc[i];
                if( !line.IsEmpty() && line.First() == '[' && line.Last() == ']' )
                  break;

                doc.RemoveLinesAt( i );
              }

              break;
            }
          }

          break;
        }

        list = list->GetNext();
      }

      Union.GetUnionOption().Write( true, "MOUSE", "EnableWrapper" );
      Union.GetUnionOption().Write( true, "MOUSE", "OnlyAxisMode" );
      Union.GetUnionOption().Write( 1.0f, "MOUSE", "SpeedMultiplier" );
      Union.GetUnionOption().Write( 1.0f, "MOUSE", "PrecisionMultiplier" );
      Union.GetUnionOption().Write( false, "MOUSE", "ForceMouseControl" );
      Union.GetUnionOption().Write( 00.5f, "MOUSE", "FixTargetDelay" );
    }
  }





  
  // Условная активация враппера
  void DoEnableMouse() {
    ClearOldMouseSection();
    
    bool enabled;
    bool onlyAxis;
    Union.GetUnionOption().Read( enabled, "MOUSE", "EnableWrapper", true );
    if( !enabled )
      return;

    DisableAcquire();
    Union.GetUnionOption().Read( focusInteruptTimeF, "MOUSE", "FixTargetDelay",    0.5f );
    Union.GetUnionOption().Read( forceMouseControl,  "MOUSE", "ForceMouseControl", false );
    Union.GetUnionOption().Read( onlyAxis,           "MOUSE", "OnlyAxisMode",      true );

    focusInteruptTime = (int)(focusInteruptTimeF * 1000.0f);
    if( forceMouseControl )
      cmd << Col16( CMD_YELLOW ) << "Warning: " << Col16( CMD_YELLOW | CMD_INT ) << "Mouse is running in the background" << Col16() << endl;

    if( CreateMouseDevice() ) {
      if( onlyAxis ) Hook_UpdateMouse_OnlyAxis.Commit();
      else           Hook_UpdateMouse.Commit();

      Hook_GetMousePos.Commit();
    }
  }






  // Steam overlay funcs
  typedef char(*LPISOVERLAYENABLED)();
  typedef int(*LPSTEAMOVERLAYISUSINGGAMEPAD)();
  typedef int(*LPSTEAMOVERLAYISUSINGKEYBOARD)();
  typedef int(*LPSTEAMOVERLAYISUSINGMOUSE)();

  LPISOVERLAYENABLED            IsOverlayEnabled;
  LPSTEAMOVERLAYISUSINGGAMEPAD  SteamOverlayIsUsingGamepad;
  LPSTEAMOVERLAYISUSINGKEYBOARD SteamOverlayIsUsingKeyboard;
  LPSTEAMOVERLAYISUSINGMOUSE    SteamOverlayIsUsingMouse;


  // Take input if overlay is visible
  //CInvoke<void(__thiscall*)(zCInput_Win32*)> Ivk_zCInput_Win32_ProcessInputEvents( ZenDef( 0x004C8BA0, 0, 0, 0x004D5700 ) );
  HOOK Hook_ProcessInputEvents PATCH_IF( &zCInput_Win32::ProcessInputEvents, &zCInput_Win32::ProcessInputEvents_Union, false );

  void zCInput_Win32::ProcessInputEvents_Union() {
    if( SteamOverlayIsUsingMouse    && SteamOverlayIsUsingMouse() )    { ClearKeyBuffer(); return; }
    if( SteamOverlayIsUsingKeyboard && SteamOverlayIsUsingKeyboard() ) { ClearKeyBuffer(); return; }
    if( SteamOverlayIsUsingMouse    && SteamOverlayIsUsingMouse() )    { ClearKeyBuffer(); return; }

    THISCALL( Hook_ProcessInputEvents )();
  }







 void CheckSteamOverlayState() {
#if ENGINE == Engine_G1 || ENGINE == Engine_G2A
    bool takeControl;
    Union.GetUnionOption().Read( takeControl, "MOUSE", "GameOverlayTakeControl", true );
    if( !takeControl )
      return;

    HMODULE module = GetModuleHandle( "GameOverlayRenderer.dll.tmp" );
    if( !module ) {
      module = GetModuleHandle( "GameOverlayRenderer.dll" );
      if( !module )
        return;
    }

    SteamOverlayIsUsingGamepad  = (LPSTEAMOVERLAYISUSINGGAMEPAD) GetProcAddress( module, "SteamOverlayIsUsingGamepad" );
    SteamOverlayIsUsingKeyboard = (LPSTEAMOVERLAYISUSINGKEYBOARD)GetProcAddress( module, "SteamOverlayIsUsingKeyboard" );
    SteamOverlayIsUsingMouse    = (LPSTEAMOVERLAYISUSINGMOUSE)   GetProcAddress( module, "SteamOverlayIsUsingMouse" );

    if( SteamOverlayIsUsingGamepad || SteamOverlayIsUsingKeyboard || SteamOverlayIsUsingMouse )
      Hook_ProcessInputEvents.Commit();
#endif
  }








  void Game_Entry() {
    ShowCursor( False );
  }

  void Game_Init() {
    CheckSteamOverlayState();
    DoEnableMouse();
  }

  void Game_Exit() {
  }

  void Game_Loop() {
  }

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
  }

  void LoadBegin() {
  }

  void LoadEnd() {
    zinput->ClearKeyBuffer();
  }

  void Game_LoadBegin_NewGame() {
    LoadBegin();
  }

  void Game_LoadEnd_NewGame() {
    LoadEnd();
  }

  void Game_LoadBegin_SaveGame() {
    LoadBegin();
  }

  void Game_LoadEnd_SaveGame() {
    LoadEnd();
  }

  void Game_LoadBegin_ChangeLevel() {
    LoadBegin();
  }

  void Game_LoadEnd_ChangeLevel() {
    LoadEnd();
  }

  void Game_LoadBegin_Trigger() {
  }
  
  void Game_LoadEnd_Trigger() {
  }

  void Game_Pause() {
  }

  void Game_Unpause() {
  }

  void Game_DefineExternals() {
  }

  // A multiplatform application reference.
  // please disable unused functions.
  // write 'false' in 'Enabled' macro.
#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals
  );
}