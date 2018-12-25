#ifndef VoiceManager_h
#define VoiceManager_h

#include <Arduino.h>
#include <base64.h>
#include <U8g2lib.h>
#include <WifiManager.h>

class VoiceManager {
  public:
    VoiceManager(const int _BUTTON_PIN, U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);
    String recordUserGuess();
    String getUserGuess();

  private:
    String speech_data;
    int button_state;
    int old_button_state;
    unsigned long time_since_sample;
    int BUTTON_PIN;
    U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *oled;

    void record_audio();
    int8_t mulaw_encode_fast(int16_t sample);
};

#endif