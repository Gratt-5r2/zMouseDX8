
// This file added in headers queue
// File: "Headers.h"

namespace GOTHIC_ENGINE {
  struct TInputDeviceState {
    bool_t Mouse;
    bool_t Keyboard;

    TInputDeviceState() {
      Init();
    }

    void Init() {
     // Mouse       = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_MOUSE     );
      Keyboard    = zinput->GetDeviceEnabled( zTInputDevice::zINPUT_KEYBOARD  );
    }

    void StopInput() {
      //zinput->SetDeviceEnabled( zTInputDevice::zINPUT_MOUSE,     False );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_KEYBOARD,  False );
      zinput->ClearKeyBuffer();
      smoothMouseState.Reset();
    }

    void RestoreInput() {
      //zinput->SetDeviceEnabled( zTInputDevice::zINPUT_MOUSE,     Mouse       );
      zinput->SetDeviceEnabled( zTInputDevice::zINPUT_KEYBOARD,  Keyboard    );
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