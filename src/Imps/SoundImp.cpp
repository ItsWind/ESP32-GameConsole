#include "src/Imps/SoundImp.h"
#include "Constants.h"
#include "FS.h"
#include "LittleFS.h"

static Audio audio;

namespace SoundImp {
  void Init() {
    audio.setPinout(SPEAKER_BCLK, SPEAKER_WSLRC, SPEAKER_DOUT);
    audio.setVolumeSteps(255);
  }

  void Update() {
    audio.loop();
  }

  void PlaySoundFromFileSystem(const char * filePath) {
    audio.connecttoFS(LittleFS, filePath);
  }

  void PlayVoiceFromWiFi(const char * say) {
    audio.connecttospeech(say, "en");
  }
}