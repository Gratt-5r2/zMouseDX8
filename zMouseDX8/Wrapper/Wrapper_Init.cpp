// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
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
        prop.diph.dwSize       = sizeof( DIPROPDWORD );
        prop.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        prop.diph.dwObj        = 0;
        prop.diph.dwHow        = DIPH_DEVICE;
        prop.dwData            = m_nBufferSize;
        if( m_lpDIMouse->SetProperty( DIPROP_BUFFERSIZE, &prop.diph ) != DI_OK ) {
          Message::Error( "#3: Input priority is not applied. Return original methods." );
          return false;
        }

        prop.diph.dwSize       = sizeof( DIPROPDWORD );
        prop.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        prop.diph.dwObj        = 0;
        prop.diph.dwHow        = DIPH_DEVICE;
        prop.dwData            = DIPROPAXISMODE_REL;
        if( m_lpDIMouse->SetProperty( DIPROP_AXISMODE, &prop.diph ) != DI_OK ) {
          Message::Error( "#4: Input properties is not applied. Return original methods." );
          return false;
        }

        // DIPROPRANGE Range;
        // Range.diph.dwSize = sizeof( Range );
        // Range.diph.dwHeaderSize = sizeof( Range.diph );
        // Range.diph.dwHow = DIPH_BYOFFSET;
        // Range.diph.dwObj = DIMOUSE_XAXIS; // The offset of the axis
        // Range.lMin = 0;
        // Range.lMax = 639;
        // if( m_lpDIMouse->SetProperty( DIPROP_RANGE, &Range.diph ) != DI_OK ) {
        //   Message::Error( "#4.1: Input properties is not applied. Return original methods." );
        //   //return false;
        // }
        // 
        // Range.diph.dwObj = DIMOUSE_YAXIS;
        // Range.lMax = 479;
        // if( m_lpDIMouse->SetProperty( DIPROP_RANGE, &Range.diph ) != DI_OK ) {
        //   Message::Error( "#4.2: Input properties is not applied. Return original methods." );
        //  // return false;
        // }

        HWND hWnd = hWndInput ? hWndInput : hWndApp;
        if( m_lpDIMouse->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) != DI_OK )
          cmd << Col16( CMD_YELLOW ) << "Warning: " << Col16( CMD_YELLOW | CMD_INT ) << "Impossible to set SetCooperativeLevel for mouse" << Col16() << endl;
      }
    }

    smoothMouseState.OnInit();
    return true;
  }


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


  int SetAcquireMouse() { return 0; }

#if ENGINE >= Engine_G2
  static HOOK Ivk_SetAcquire AS_IF( &SetAcquireMouse, &SetAcquireMouse, false );
#else
  static HOOK Ivk_SetAcquire AS_IF( &SetAcquire, &SetAcquireMouse, false );
#endif
  void DisableAcquire() {
    Ivk_SetAcquire.Commit();
  }


  HOOK Hook_UpdateMouse_OnlyAxis PATCH_IF( UpdateMouse, UpdateMouse_OnlyAxis, false );
  HOOK Hook_UpdateMouse_RawMode PATCH_IF( UpdateMouse, UpdateMouse_RawMode, false );
  HOOK Hook_UpdateMouse PATCH_IF( UpdateMouse, UpdateMouse, false );


  static void ClearOldMouseSection() {
    if( Union.GetUnionOption().IsExists( "MOUSE", "ApplyFix" ) ) {
      auto& blocks = Union.GetUnionOption().lstBlocks;

      auto list = blocks.GetNext();
      while( list ) {
        auto& block = list->GetData();

        if( block->sBlockName == "MOUSE" ) {
          block->lstValues.DeleteListDatas();
          blocks.Delete( block );

          auto& doc = Union.GetUnionOption().mDocument;
          for( uint i = 0; i < doc.GetNum(); i++ ) {
            if( (A doc[i]).Shrink() == "[MOUSE]" ) {
              doc.RemoveLinesAt( i );

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

      Union.GetUnionOption().Write( true,  "MOUSE", "EnableWrapper" );
      Union.GetUnionOption().Write( true,  "MOUSE", "OnlyAxisMode" );
      Union.GetUnionOption().Write( 1.0f,  "MOUSE", "SpeedMultiplier" );
      Union.GetUnionOption().Write( 1.0f,  "MOUSE", "PrecisionMultiplier" );
      Union.GetUnionOption().Write( false, "MOUSE", "ForceMouseControl" );
      Union.GetUnionOption().Write( 0.5f,  "MOUSE", "FixTargetDelay" );
    }
  }


  void DoEnableMouse() {
    ClearOldMouseSection();
    
    bool enabled;
    bool onlyAxis;
    Union.GetUnionOption().Read( enabled, "MOUSE", "EnableWrapper", true );
    if( !enabled )
      return;

    DisableAcquire();
    Union.GetUnionOption().Read( focusInteruptTimeF,   "MOUSE", "FixTargetDelay",    0.5f );
    Union.GetUnionOption().Read( forceMouseControl,    "MOUSE", "ForceMouseControl", false );
    Union.GetUnionOption().Read( onlyAxis,             "MOUSE", "OnlyAxisMode",      true );
    Union.GetUnionOption().Read( NoDirectXModeEnabled, "MOUSE", "NoDirectXAxisMode", false );

    focusInteruptTime = (int)(focusInteruptTimeF * 1000.0f);
    if( forceMouseControl )
      cmd << Col16( CMD_YELLOW ) << "Warning: " << Col16( CMD_YELLOW | CMD_INT ) << "Mouse is running in the background" << Col16() << endl;
    else {
      RECT rect;
      rect.left   = 0;
      rect.right  = VID_MAX_XDIM;
      rect.top    = 0;
      rect.bottom = VID_MAX_YDIM;
      ClipCursor( &rect );
    }

    if( NoDirectXModeEnabled ) {
      TCursorTracker::GetInstance().Init();
      smoothMouseState.OnInit();
      Hook_UpdateMouse_RawMode.Commit();
      Hook_GetMousePos.Commit();
    }
    else if( CreateMouseDevice() ) {
      if( onlyAxis ) {
        Hook_UpdateMouse_OnlyAxis.Commit();
      }
      else
        Hook_UpdateMouse.Commit();

      Hook_GetMousePos.Commit();
    }
  }
  
#if ENGINE <= Engine_G1A
  HOOK Hook_zCInput_Win32_SetState PATCH_IF( &zCInput_Win32::SetState, &zCInput_Win32::SetState_Union, ReadOption<bool>( "PARAMETERS", "Gothic2_Control" ) );

  bool ExpectedDialog() {
    if( !player )
      return false;

    return player->GetWeaponMode() == NPC_WEAPON_NONE && player->GetFocusNpc();
  }

  int zCInput_Win32::SetState_Union( unsigned short logicalID, int state ) {
    static bool leftToggled = false;
    if( logicalID == GAME_ACTION ) {
      if( zinput->GetMouseButtonPressedLeft() && ExpectedDialog() ) {
        if( !leftToggled )
          leftToggled = true;
        else
          return 0;
      }
      else
        if( leftToggled )
          leftToggled = false;
    }

    return THISCALL( Hook_zCInput_Win32_SetState )(logicalID, state);
  };
#endif
}