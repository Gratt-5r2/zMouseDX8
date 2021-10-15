// Supported with union (c) 2020 Union team
// Union SOURCE file
#include <intrin.h>

#pragma intrinsic(_ReturnAddress)

namespace GOTHIC_ENGINE {
  static ThreadLocker locker;

  static const int VidCenterX   = VID_MAX_XDIM / 2;
  static const int VidCenterY   = VID_MAX_YDIM / 2;
  static bool NoDirectXAxisMode = ReadOption<bool>( "MOUSE", "NoDirectXAxisMode" );

  TCursorTracker::TCursorTracker() {
    AverageX = 0;
    AverageY = 0;
    VectorX = 0;
    VectorY = 0;
    VirtualPositionX = VidCenterX;
    VirtualPositionX = VidCenterY;
  }

  void TCursorTracker::Track( const int& x, const int& y ) {
    ThreadLockerSingle lockerSingle( locker );
    VectorX          += x;
    VectorY          += y;
    VirtualPositionX += x;
    VirtualPositionY += y;

    AverageX = (AverageX + (x - VidCenterX)) / 2;
    AverageY = (AverageY + (y - VidCenterY)) / 2;

    if( VirtualPositionX < 0 )            VirtualPositionX = 0;
    if( VirtualPositionY < 0 )            VirtualPositionY = 0;
    if( VirtualPositionX > VID_MAX_XDIM ) VirtualPositionX = VID_MAX_XDIM;
    if( VirtualPositionY > VID_MAX_YDIM ) VirtualPositionY = VID_MAX_YDIM;
  }

  void TCursorTracker::GetTracking( int& x, int& y ) {
    ThreadLockerSingle lockerSingle( locker );
    x = VectorX;
    y = VectorY;

    VectorX = 0;
    VectorY = 0;
  }

  void TCursorTracker::GetCursorVirtualPos( int& x, int& y ) {
    ThreadLockerSingle lockerSingle( locker );
    x = VirtualPositionX;
    y = VirtualPositionY;
  }

  void TCursorTracker::GetAverageSpeed( int& x, int& y ) {
    ThreadLockerSingle lockerSingle( locker );
    x = AverageX;
    y = AverageY;
  }

  void TCursorTracker::SetCursorVirtualPos( const int& x, const int& y ) {
    ThreadLockerSingle lockerSingle( locker );
    VirtualPositionX = x;
    VirtualPositionY = y;
  }

  BOOL WINAPI Hooked_GetCursorPos( LPPOINT lppoint ) {

    TCursorTracker::GetInstance().GetCursorVirtualPos( (int&)lppoint->x, (int&)lppoint->y );
    return TRUE;
  }

  BOOL WINAPI Hooked_SetCursorPos( int x, int y ) {
    TCursorTracker::GetInstance().SetCursorVirtualPos( x, y );
    return TRUE;
  }

  HOOK Hook_GetCursorPos AS_IF( 0, &Hooked_GetCursorPos, false );
  HOOK Hook_SetCursorPos AS_IF( 0, &Hooked_SetCursorPos, false );

  void TCursorTracker::Init() {
    HMODULE module = GetModuleHandle( "User32.dll" );
    void* func_GetCursorPos = GetProcAddress( module, "GetCursorPos" );
    void* func_SetCursorPos = GetProcAddress( module, "SetCursorPos" );
    Hook_GetCursorPos.Attach( func_GetCursorPos, &Hooked_GetCursorPos );
    Hook_SetCursorPos.Attach( func_SetCursorPos, &Hooked_SetCursorPos );
    Thread thread( &TCursorTracker::TrackerThread );
    thread.Detach();
  }

  void TCursorTracker::MatchChanges() {
    GetTracking( PositionX, PositionY );
  }

  void TCursorTracker::ResetMotion() {
    GetTracking( PositionX, PositionY );
    PositionX = 0;
    PositionY = 0;
  }

  void TCursorTracker::GetCursorPos( int& x, int& y ) {
    x = PositionX;
    y = PositionY;
  }

  void TCursorTracker::TrackerThread() {
    while( true ) {
      if( IsMouseDeviceEnabled() ) {
        POINT point;
        Hook_GetCursorPos( &point );
        Hook_SetCursorPos( VidCenterX, VidCenterY );
        GetInstance().Track( point.x - VidCenterX, point.y - VidCenterY );
      }

      Sleep( 2 );
    }
  }

  TCursorTracker& TCursorTracker::GetInstance() {
    static TCursorTracker instance;
    return instance;
  }


  void UpdateMouse_RawMode() {
    static bool newMouseEnabled = true;
    bool mouseEnabled = zinput->GetDeviceEnabled( zINPUT_MOUSE );

    if( newMouseEnabled && !mouseEnabled ) {
      newMouseEnabled = false;
    }
    else if( !newMouseEnabled && mouseEnabled ) {
      newMouseEnabled = true;
      SetForegroundWindow( GetDesktopWindow() );
      SetForegroundWindow( hWndApp );
    }

    if( !mouseEnabled && !forceMouseControl )
      return;

    smoothMouseState.Reset();

    Hook_UpdateMouse_RawMode();
    if( !UseIdleDeviceData ) {
      int x, y;
      TCursorTracker::GetInstance().GetCursorPos( x, y );
      smoothMouseState.SetX( (float)x * 1.0f ); // (Union.GetEngineVersion() < Engine_G2 ? 0.75f : 1.0f) );
      smoothMouseState.SetY( (float)y * 1.0f );
      smoothMouseState.Smooth();
      smoothMouseState.SetZ( (float)mouseState.zpos );
    }
    else
      zinput->ClearKeyBuffer();
  }

  bool IsMouseDeviceEnabled() {
    ThreadLockerSingle lockerSingle( locker );
    bool result = MouseDeviceEnabled;
    return result;
  }
  
  void SetMouseDeviceEnabled( bool enable ) {
    ThreadLockerSingle lockerSingle( locker );
    MouseDeviceEnabled = enable;
  }





#define ROT3DRATIO (0.008f)

#define PUSHFRAMETIME(t)                           \
  float ft               = ztimer->frameTimeFloat; \
  float mf               = ztimer->factorMotion;   \
  ztimer->frameTimeFloat = (t);                    \
  ztimer->factorMotion   = 1.0f;

#define POPFRAMETIME           \
  ztimer->frameTimeFloat = ft; \
  ztimer->factorMotion   = mf;




  HOOK Hook_zCAICamera_CheckKeys PATCH_IF( &zCAICamera::CheckKeys, &zCAICamera::CheckKeys_Union, NoDirectXAxisMode );

  void zCAICamera::CheckKeys_Union() {
    PUSHFRAMETIME( 1.0f );
    THISCALL( Hook_zCAICamera_CheckKeys )();
    POPFRAMETIME;
  }




  HOOK Hook_oCAIHuman_PC_Turnings PATCH_IF( &oCAIHuman::PC_Turnings, &oCAIHuman::PC_Turnings_Union, NoDirectXAxisMode );

  void oCAIHuman::PC_Turnings_Union( int forceRotation ) {
    if( !Pressed( GAME_LEFT ) && !Pressed( GAME_RIGHT ) ) {
      PUSHFRAMETIME(2.0f);
      THISCALL( Hook_oCAIHuman_PC_Turnings )(forceRotation);
      POPFRAMETIME;
    }
    else
      THISCALL( Hook_oCAIHuman_PC_Turnings )(forceRotation);
  };


  HOOK Hook_oCAIHuman_PC_Diving PATCH_IF( &oCAIHuman::PC_Diving, &oCAIHuman::PC_Diving_Union, NoDirectXAxisMode );

  void oCAIHuman::PC_Diving_Union() {
    if( !Pressed( GAME_UP ) && !Pressed( GAME_DOWN ) ) {
      PUSHFRAMETIME( 5.0f );
      THISCALL( Hook_oCAIHuman_PC_Diving )();
      POPFRAMETIME;
    }
    else
      THISCALL( Hook_oCAIHuman_PC_Diving )();
  }



  HOOK Hook_oCAniCtrl_Human_Turn PATCH_IF( &oCAniCtrl_Human::Turn, &oCAniCtrl_Human::Turn_Union, NoDirectXAxisMode );

  float oCAniCtrl_Human::Turn_Union( float dir, int playani ) {
    static uint LastAniTime = 0;

    if( zBindPressed( GAME_LEFT ) || zBindPressed( GAME_RIGHT ) ) {
      // pass
    }
    else if( dir != 0 ) {
      playani = abs( dir * 0.9f ) > ztimer->frameTime;
      uint now = Timer::GetTime();
      if( playani )
        LastAniTime = now;
      else if( now - LastAniTime < 100 ) {
        playani = True;
        LastAniTime = Timer::GetTime();
      }

      PUSHFRAMETIME( 2.0f );
      float degree = THISCALL( Hook_oCAniCtrl_Human_Turn )(dir, playani);
      POPFRAMETIME;
      return degree;
    }

    return THISCALL( Hook_oCAniCtrl_Human_Turn )(dir, playani);
  }
}