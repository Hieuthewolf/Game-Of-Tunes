#ifndef WifiManager_h
#define WifiManager_h

#include <WiFiClientSecure.h>

#define RESPONSE_TIMEOUT      6000

class WifiManager {
  public:
    WifiManager();

    void postNewState(String _gameState);
    void postUsername(String _userName);
    String getOtherPlayersUserNames();
    String getSongIndexes();
    String getHint(int _hintNumber, int _songIndex);
    String checkPlayerGuess(int _songIndex, String _userGuess);
    String updateLeaderBoard(String _username, int _playerScore);
    String getLeaderBoard();
    String getNumberOfLockedPlayers();
    void   adjustLockedPlayers(String _lockedPlayerStatus);
    String getLatestState();
    void   resetSongs();
  private:
};

#endif