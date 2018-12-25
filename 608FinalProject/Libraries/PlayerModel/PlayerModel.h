#ifndef PlayerModel_h
#define PlayerModel_h

#include <UsernameCreator.h>

class PlayerModel {
  public:
    PlayerModel(U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);

    String createUsername(float _yAngle, int _leftButton, int _rightButton);
    int    playerScore();
    String playerGuess();
    String username();

    void   setPlayerGuess(String _playerGuess);
    void   setPlayerScore(int _newPlayerScore);
    void   setUsername();

  private:
    int    mPlayerScore;
    String mUsername;
    String mPlayerGuess;

    UsernameCreator *usernameCreator;
    U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *oled;
};

#endif