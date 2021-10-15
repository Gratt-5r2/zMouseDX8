// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
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
            int state = IK_PRESS( data.dwData );
            mouseState.buttonPressedLeft = state;
            wrapperMouseState.buttonPressedLeft = state;
#if ENGINE != Engine_G1
            SetMouseKeyStateAndInsert( MOUSE_BUTTONLEFT, data.dwData );
#endif
            break;
          }
          case DIMOFS_BUTTON1:
          {
            int state = IK_PRESS( data.dwData );
            mouseState.buttonPressedRight = state;
            wrapperMouseState.buttonPressedRight = state;
#if ENGINE != Engine_G1
            SetMouseKeyStateAndInsert( MOUSE_BUTTONRIGHT, data.dwData );
#endif
            break;
          }
          case DIMOFS_BUTTON2:
          {
            int state = IK_PRESS( data.dwData );
            mouseState.buttonPressedMid = state;
            wrapperMouseState.buttonPressedMid = state;
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

    if( UseIdleDeviceData )
      zinput->ClearKeyBuffer();
  }
}