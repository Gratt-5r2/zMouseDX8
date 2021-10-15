// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  long __cdecl winDialogParamFinal( unsigned short, int( __stdcall* )(struct HWND__*, unsigned int, unsigned int, long), unsigned long );
#if ENGINE == Engine_G2A
  HOOK Hook_winDialogParamFinal PATCH( 0x00506230, &winDialogParamFinal );
#elif ENGINE == Engine_G2
  HOOK Hook_winDialogParamFinal PATCH( 0x00503530, &winDialogParamFinal );
#elif ENGINE == Engine_G1A
  HOOK Hook_winDialogParamFinal PATCH( 0x0050A090, &winDialogParamFinal );
#elif ENGINE == Engine_G1
  HOOK Hook_winDialogParamFinal PATCH( 0x004F7760, &winDialogParamFinal );
#endif
  long __cdecl winDialogParamFinal( unsigned short a0, int( __stdcall* a1 )(struct HWND__*, unsigned int, unsigned int, long), unsigned long a2 ) {
    UpdateMouseFocus( true );
    return Hook_winDialogParamFinal( a0, a1, a2 );
  }
}