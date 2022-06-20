// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
#define MOUSE_KEY_LEFT_G1  2050
#define MOUSE_KEY_MID_G1   2051
#define MOUSE_KEY_RIGHT_G1 2052
#define MOUSE_WHEELUP_G1   2057
#define MOUSE_WHEELDOWN_G1 2058

  typedef void* (*LPGETHANDLE)(void);
  typedef void( __thiscall* LPGETMOUSEPOS )(zCInput_Win32*, float&, float&, float&);

  bool* keyevent           = (bool*)         ZenDef( 0x0086CCC8, 0x008B27C0, 0x008C3020, 0x008D1678 );
  bool* keytoggle          = (bool*)         ZenDef( 0x0086CED0, 0x008B29D4, 0x008C3260, 0x008D18B8 );
  zCArray<int>& keybuffer  = *(zCArray<int>*)ZenDef( 0x0086D2DC, 0x008B2E00, 0x008C36F0, 0x008D1D50 );
  zTMouseState& mouseState = *(zTMouseState*)ZenDef( 0x0086CCAC, 0x008B27A8, 0x008C3004, 0x008D165C );
  TRealMousePosition smoothMouseState;
  
  static float&  postMultiplierX      = *(real32*)ZenDef( 0x00836538, 0x0087C15C, 0x0088BFE0, 0x0089A148 );
  static float&  postMultiplierY      = *(real32*)ZenDef( 0x0083653C, 0x0087C160, 0x0088BFE4, 0x0089A14C );
  static bool_t& postInverseX         = *(bool_t*)ZenDef( 0x0086D304, 0x008B2E28, 0x008C3718, 0x008D1D78 );
  static bool_t& postInverseY         = *(bool_t*)ZenDef( 0x0086D308, 0x008B2E2C, 0x008C371C, 0x008D1D7C );
  static bool    forceMouseControl    = false;
  static float   focusInteruptTimeF   = 0.5f;
  static int     focusInteruptTime    = 500;
  static bool    UseIdleDeviceData    = false;
  static bool    MouseDeviceEnabled   = false;
  static bool    NoDirectXModeEnabled = false;

  static bool IsMouseDeviceEnabled();
  static void SetMouseDeviceEnabled( bool enable );

  static LPDIRECTINPUT8        m_lpDInput     = Null;
  LPDIRECTINPUTDEVICE8A        m_lpDIMouse    = Null;
  static LPDIRECTINPUTDEVICE8A m_lpDIJoystick = Null;
  static const int             m_nBufferSize  = 16;

  void UpdateMouse_RawMode();
  void UpdateMouse_OnlyAxis();
  void UpdateMouse();

  // Steam overlay funcs
  typedef char(*LPISOVERLAYENABLED)();
  typedef int(*LPSTEAMOVERLAYISUSINGGAMEPAD)();
  typedef int(*LPSTEAMOVERLAYISUSINGKEYBOARD)();
  typedef int(*LPSTEAMOVERLAYISUSINGMOUSE)();

  LPISOVERLAYENABLED            IsOverlayEnabled;
  LPSTEAMOVERLAYISUSINGGAMEPAD  SteamOverlayIsUsingGamepad;
  LPSTEAMOVERLAYISUSINGKEYBOARD SteamOverlayIsUsingKeyboard;
  LPSTEAMOVERLAYISUSINGMOUSE    SteamOverlayIsUsingMouse;

  struct TCursorTracker {
    int VirtualPositionX;
    int VirtualPositionY;
    int VectorX;
    int VectorY;
    int AverageX;
    int AverageY;
    int PositionX;
    int PositionY;

    void Track( const int& x, const int& y );
    void GetTracking( int& x, int& y );
    void GetCursorVirtualPos( int& x, int& y );
    void GetAverageSpeed( int& x, int& y );
    void SetCursorVirtualPos( const int& x, const int& y );
    void Init();
    void MatchChanges();
    void ResetMotion();
    void GetCursorPos( int& x, int& y );
    static void TrackerThread();
    static TCursorTracker& GetInstance();

  private:
    TCursorTracker();
  };

#if ENGINE == Engine_G1 || ENGINE == Engine_G2A
  void NinjaGFAHelper();
#endif
}