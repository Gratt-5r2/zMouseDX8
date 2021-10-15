// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
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

      DIMOUSESTATE2 dims2;
      m_lpDIMouse->GetDeviceState( sizeof( dims2 ), &dims2 );

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
            smoothMouseState.SetZ( (float)(int)data.dwData );
            break;
          case DIMOFS_BUTTON0:
            wrapperMouseState.buttonPressedLeft = IK_PRESS( data.dwData );
            break;
          case DIMOFS_BUTTON1:
            wrapperMouseState.buttonPressedRight = IK_PRESS( data.dwData );
            break;
          case DIMOFS_BUTTON2:
            wrapperMouseState.buttonPressedMid = IK_PRESS( data.dwData );
            break;
        }
      }
      else if( elements == 0 ) done = True;
    }

    smoothMouseState.Smooth();
    Hook_UpdateMouse_OnlyAxis();

    if( UseIdleDeviceData )
      zinput->ClearKeyBuffer();
  }
}