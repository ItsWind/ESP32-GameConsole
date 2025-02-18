#pragma once

#include "Input.h"
#include "TFTImp.h"

namespace MenuImp {
  struct Menu {
    //void (*Update)(unsigned long dt);
    void (*Draw)();
  };

  extern Menu MenuList[1];
  extern Menu * CurrentMenu;

  void SetMenuTo(int8_t index);
}