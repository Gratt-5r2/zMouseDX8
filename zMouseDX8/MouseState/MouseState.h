// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
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
}