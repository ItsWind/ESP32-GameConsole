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

  class SplashMenu : public Menu {
    private:
      unsigned long screenTime;
    public:
      void Init() override;
      void Destroy() override;
      void Update(unsigned long dt) override;
      void Draw() override;
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

  class TextListMenu : public Menu {
    private:
      uint8_t currentTextIndex;
      uint8_t textListCount;
      const char ** textList;
    public:
      void Init() override;
      void Destroy() override;
      void Update(unsigned long dt) override;
      void Draw() override;
  };

  /*class GamesMenu : public TextListMenu {
    public:
      void Init() override;
      void Destroy() override;
      void Update(unsigned long dt) override;
      void Draw() override;
  };*/

  void SetMenu(Menu * newMenu);
  
  extern Menu * CurrentMenu;
}