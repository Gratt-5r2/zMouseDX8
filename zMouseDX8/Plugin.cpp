
// This file added in headers queue
// File: "Sources.h"

namespace GOTHIC_ENGINE {
  

  void Game_Entry() {
    //while( ShowCursor( False ) >= -1 );
    ShowCursor( False );
  }

  void Game_Init() {
    CheckSteamOverlayState();
    DoEnableMouse();
    TInputDeviceState::GetInstance().Init();
  }

  void Game_Exit() {
    if( m_lpDIMouse ) {
      m_lpDIMouse->Unacquire();
      m_lpDIMouse->Release();
      m_lpDIMouse = Null;
    }
  }


  void UpdateMouseFocus( const bool& emergencyStopInput = false ) {
    if( forceMouseControl && !emergencyStopInput )
      return;
    
    static Timer timer;
    timer.ClearUnused();
    
    static bool windowIsActive = false;
    if( GetForegroundWindow() == hWndApp && !emergencyStopInput ) {
      if( !windowIsActive ) {
        // Ivk_SetAcquire();
        if( m_lpDIMouse )
          m_lpDIMouse->Acquire();
    
        SetMouseDeviceEnabled( true );
        windowIsActive = true;
        // cmd << "Restore input" << endl;
        TInputDeviceState::GetInstance().RestoreInput();
    
        if( NoDirectXModeEnabled ) {
          Hook_SetCursorPos( VidCenterX, VidCenterY );
          while( ShowCursor( False ) >= 0 );
        }
      }
    
      if( UseIdleDeviceData && timer[0u].Await( 250 ) ) {
        timer[0u].Delete();
        UseIdleDeviceData = false;
      }
    }
    else {
      if( windowIsActive ) {
        if( m_lpDIMouse )
          m_lpDIMouse->Unacquire();
    
        SetMouseDeviceEnabled( false );
        windowIsActive = false;
        // cmd << "Stop imput" << endl;
        TInputDeviceState::GetInstance().StopInput();
        UseIdleDeviceData = true;
    
        if( NoDirectXModeEnabled ) {
          while( ShowCursor( True ) < 0 );
        }
      }
    }
  }

  HOOK Hook_zCBinkPlayer_PlayFrame PATCH_IF( &zCBinkPlayer::PlayFrame, &zCBinkPlayer::PlayFrame_Union, CHECK_THIS_ENGINE );
  
  int zCBinkPlayer::PlayFrame_Union() {
    UpdateMouseFocus();
    return THISCALL( Hook_zCBinkPlayer_PlayFrame )();
  }

  void Game_PreLoop() {
    TCursorTracker::GetInstance().MatchChanges();
  }

  void Game_Loop() {
    UpdateMouseFocus();
  }

  void Game_PostLoop() {
  }

  void Game_MenuLoop() {
    UpdateMouseFocus();
  }

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
  }

  void LoadBegin() {
  }

  void LoadEnd() {
    bool clearKeyBufferOnLoad = true;
    Union.GetUnionOption().Read( clearKeyBufferOnLoad, "DEBUG", "ClearKeyBufferOnLoad", clearKeyBufferOnLoad );

    if( clearKeyBufferOnLoad ) {
      zinput->ClearKeyBuffer();
      TCursorTracker::GetInstance().ResetMotion();
    }
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
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
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