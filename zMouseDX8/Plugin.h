
// This file added in headers queue
// File: "Headers.h"

namespace GOTHIC_ENGINE {
  struct TInputDeviceState {
    bool_t Mouse;
    bool_t Keyboard;
    bool_t Joystick[4];

    void Init() {
      Mouse       = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_MOUSE     );
      Keyboard    = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_KEYBOARD  );
      Joystick[0] = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK1 );
      Joystick[1] = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK2 );
      Joystick[2] = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK3 );
      Joystick[3] = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK4 );
    }

    void StopInput() {
      Init();
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_MOUSE,     False );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_KEYBOARD,  False );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK1, False );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK2, False );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK3, False );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK4, False );
      zinput->ClearKeyBuffer();
      smoothMouseState.Reset();
    }

    void RestoreInput() {
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_MOUSE,     Mouse       );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_KEYBOARD,  Keyboard    );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK1, Joystick[0] );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK2, Joystick[1] );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK3, Joystick[2] );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_JOYSTICK4, Joystick[3] );
      zinput->ClearKeyBuffer();
    }

    static TInputDeviceState& GetInstance() {
      static TInputDeviceState instance;
      return instance;
    }
  };

  template<class T>
  T ReadOption( const string& section, const string& parameter ) {
    T result;
    Union.GetSysPackOption().Read( result, section, parameter );
    return result;
  }
}