
// This file added in headers queue
// File: "Headers.h"

namespace NAMESPACE {
  struct zTMouseState {
    int xpos;
    int ypos;
    int zpos;
    int buttonPressedLeft;
    int buttonPressedMid;
    int buttonPressedRight;
  };

  struct TRealMousePosition {
    bool32 PrecisionEnabled;
    float  SpeedMultX;
    float  SpeedMultY;
    float  PrecisionMult;
    float  x;
    float  y;
    float  z;

    float Length();
    void  SetX( const float& v );
    void  SetY( const float& v );
    void  SetZ( const float& v );
    float GetX();
    float GetY();
    float GetZ();
    void  Reset();
    void  Smooth();
    void  OnInit();
  };






  POINT CalcGothicMemRange() {
    MODULEINFO info;
    HANDLE proc = GetCurrentProcess();
    HMODULE module = CPlugin::GetGothicModule();
    GetModuleInformation( proc, module, &info, sizeof( MODULEINFO ) );
    uint moduleSize = info.SizeOfImage;
    POINT memRange;
    memRange.x = (uint)info.lpBaseOfDll;
    memRange.y = info.SizeOfImage;
    return memRange;
  }

  void GetGothicMemRange( uint& base, uint& length ) {
    static POINT memRange = CalcGothicMemRange();
    base   = memRange.x;
    length = memRange.y;
  }

  bool IsCall( const byte& instruction ) {
    return
      instruction == 0xE8 /*||
      instruction == 0xFF ||
      instruction == 0x9A*/;
  }

  void MemSearchCall( Array<uint>& absolutes, Array<uint>& offsets, const uint& address ) {
    uint begin;
    uint length;
    GetGothicMemRange( begin, length );
    uint end = begin + length - 4;

    for( uint i = begin; i < end; i++ ) {

      byte instruction = *(uint*)i;
      if( IsCall( instruction ) ) {
        uint& value = *(uint*)(i + 1);
        uint offset = i + value + 5;

        if( offset == address ) {
          offsets.Insert(i + 1);
          i += 4;
        }
        else if( value == address ) {
          absolutes.Insert(i + 2);
          i += 5;
        }

        continue;
      }
      
      uint& value = *(uint*)i;
      if( value == address ) {
        absolutes.Insert( i );
        i += 3;
      }
    }
  }

  void MemPatchCall( const uint& from, const uint& to ) {
    Array<uint> absolutes, offsets;
    MemSearchCall( absolutes, offsets, from );

    for( uint i = 0; i < absolutes.GetNum(); i++ ) {
      uint in = absolutes[i];
      SystemPack::TMemUnlocker unlocker( (void*)in, 5, True );
      uint& address = *(uint*)in;
      address = to;
    }

    for( uint i = 0; i < offsets.GetNum(); i++ ) {
      uint in = offsets[i];
      SystemPack::TMemUnlocker unlocker( (void*)in, 5, True );
      uint& address = *(uint*)in;
      address = to - (in + 4);
    }
  }
}