// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  // Take input if overlay is visible
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
}