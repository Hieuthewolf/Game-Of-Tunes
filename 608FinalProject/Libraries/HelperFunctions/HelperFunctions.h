#include <Arduino.h>
#include <U8g2lib.h>

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 148

#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI

class HelperFunctions {
  public:
    HelperFunctions();//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);

    void prettyPrint(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display);
    void oled_print(String input, int startx, int starty, const uint8_t* font, SCREEN &oled);
    String parse_username(String text, int index_of_commma);

  private:
};