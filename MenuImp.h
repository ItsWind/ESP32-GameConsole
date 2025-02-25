#pragma once

#include <Arduino.h>

namespace MenuImp {
  class Menu {
    public:
      virtual void Init();
      virtual void Destroy();
      virtual void Update(unsigned long dt);
      virtual void Draw();
  };

  class MessageMenu : public Menu {
    private:
      const char * messageToDisplay;
    public:
      MessageMenu(const char * message);
      void Init() override;
      void Destroy() override;
      void Update(unsigned long dt) override;
      void Draw() override;
  };

  class MainMenu : public Menu {
    public:
      void Init() override;
      void Destroy() override;
      void Update(unsigned long dt) override;
      void Draw() override;
  };

  void SetMenu(Menu * newMenu);

  extern Menu * CurrentMenu;
}