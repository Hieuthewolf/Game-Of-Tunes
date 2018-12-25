#ifndef UsernameCreator_h
#define UsernameCreator_h

#include <HelperFunctions.h>
#include <WifiManager.h>

class UsernameCreator {
  public:
    UsernameCreator(U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);

    String update(float _angle, int _leftButton, int _rightButton);
    String getUsername();

  private:
  	String alphabet=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  	int    scrollingThreshold = 195;
  	float  angleThreshold      = 0.4;

  	int charIndex;
  	int state;
  	unsigned long scrollingTimer;
  	String  queryString;
  	String  usernameMessage;
    WifiManager *Sever;
    HelperFunctions *Helpers;
  	U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *oled;
};

#endif