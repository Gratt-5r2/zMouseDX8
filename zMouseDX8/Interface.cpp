// Interface queue
#include "UnionAfx.h"

#pragma comment(lib, "C:\\ProgramData\\Union\\SDK\\DirectX8\\lib\\dxguid.lib")
#pragma comment(lib, "C:\\ProgramData\\Union\\SDK\\DirectX8\\lib\\dinput8.lib")

#define DIRECTINPUT_VERSION 0x0800
#define DllExport __declspec(dllexport)
#define IK_PRESS(dw) ((dw & 0x80) == 0x80)
#include <DirectX8\include\dinput.h>



EXTERN_C {
  struct zTWrapperMouseState {
    int xpos;
    int ypos;
    int zpos;
    int buttonPressedLeft;
    int buttonPressedMid;
    int buttonPressedRight;
  };

  DllExport zTWrapperMouseState wrapperMouseState;
}



EXTERN_C {
  void* ImportMouseFixSymbol( const char* symName ) {
    HMODULE module = CPlugin::FindModule( "zMouseFix.dll" );
    if( !module )
      return Null;
    
    return GetProcAddress( module, symName );
  }

  zTWrapperMouseState& GetWrapperMouseKeyState() {
    static zTWrapperMouseState* instance = (zTWrapperMouseState*)ImportMouseFixSymbol( "wrapperMouseState" );
    return *instance;
  }
}


// Check executed engine with current source code
#define CHECK_THIS_ENGINE (Union.GetEngineVersion() == ENGINE)
#define Engine_G1  1
#define Engine_G1A 2
#define Engine_G2  3
#define Engine_G2A 4



HWND hWndInput = Null;

extern "C" __declspec( dllexport ) void SetInputWindow( HWND hWnd ) {
  hWndInput = hWnd;
}



// Include headers
#ifdef __G1
#define GOTHIC_ENGINE Gothic_I_Classic
#define ENGINE Engine_G1
#include "Headers.h"
#endif
#ifdef __G1A
#define GOTHIC_ENGINE Gothic_I_Addon
#define ENGINE Engine_G1A
#include "Headers.h"
#endif
#ifdef __G2
#define GOTHIC_ENGINE Gothic_II_Classic
#define ENGINE Engine_G2
#include "Headers.h"
#endif
#ifdef __G2A
#define GOTHIC_ENGINE Gothic_II_Addon
#define ENGINE Engine_G2A
#include "Headers.h"
#endif



// Include source files (with same as header parameters)
#ifdef __G1
#define GOTHIC_ENGINE Gothic_I_Classic
#define ENGINE Engine_G1
#define UPDATEMOUSE 0x004C7F40
#include "Sources.h"
#endif
#ifdef __G1A
#define GOTHIC_ENGINE Gothic_I_Addon
#define ENGINE Engine_G1A
#define UPDATEMOUSE 0x004D7A90
#include "Sources.h"
#endif
#ifdef __G2
#define GOTHIC_ENGINE Gothic_II_Classic
#define ENGINE Engine_G2
#define UPDATEMOUSE 0x004D17D0
#include "Sources.h"
#endif
#ifdef __G2A
#define GOTHIC_ENGINE Gothic_II_Addon
#define ENGINE Engine_G2A
#define UPDATEMOUSE 0x004D3D90
#include "Sources.h"
#endif