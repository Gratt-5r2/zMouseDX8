// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  // int __cdecl Apply_Options_Video(void)
  int Apply_Options_Video();

  HOOK Ivk_Apply_Options_Video AS( &Apply_Options_Video, &Apply_Options_Video );

  __declspec(noinline) int Apply_Options_Video() {
    if( zinput && zinput->GetDeviceEnabled( zINPUT_MOUSE ) )
      ShowCursor( False );

    return Ivk_Apply_Options_Video();
  }


  float TRealMousePosition::Length() {
    return sqrt( x * x + y * y );
  }


  void TRealMousePosition::SetX( const float& v ) {
    x = v;
    int state = (int)ceil( x );
    mouseState.xpos = state;
    wrapperMouseState.xpos = state;
  }


  void TRealMousePosition::SetY( const float& v ) {
    y = v;
    int state = (int)ceil( y );
    mouseState.ypos = state;
    wrapperMouseState.ypos = state;
  }


  void TRealMousePosition::SetZ( const float& v ) {
    z = v;
    int state = (int)ceil( z );
    mouseState.zpos = state;
    wrapperMouseState.zpos = state;
  }


  float TRealMousePosition::GetX() {
    return x * SpeedMultX;
  }


  float TRealMousePosition::GetY() {
    return y * SpeedMultY;
  }


  float TRealMousePosition::GetZ() {
    return z;
  }


  void TRealMousePosition::Reset() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    mouseState.xpos = 0;
    mouseState.ypos = 0;
  }


  void TRealMousePosition::Smooth() {
    if( PrecisionEnabled ) {
      float length = Length();
      if( length < PrecisionMult ) {
        float smoothMult = sin( length / PrecisionMult );
        x *= smoothMult;
        y *= smoothMult;
      }
    }
  }


  void TRealMousePosition::OnInit() {
    Union.GetUnionOption().Read( SpeedMultX,    "MOUSE", "SpeedMultiplierX",    1.0f );
    Union.GetUnionOption().Read( SpeedMultY,    "MOUSE", "SpeedMultiplierY",    1.0f );
    Union.GetUnionOption().Read( PrecisionMult, "MOUSE", "PrecisionMultiplier", 1.0f );
    if( SpeedMultX == 0.0f )
      SpeedMultX = 1.0f;

    if( SpeedMultY == 0.0f )
      SpeedMultY = 1.0f;

    if( PrecisionMult > 0.0f )
      PrecisionEnabled = True;
  }
}