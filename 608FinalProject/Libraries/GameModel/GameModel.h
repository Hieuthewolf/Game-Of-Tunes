#ifndef GameModel_h
#define GameModel_h

#include <DFRobotDFPlayerMini.h>
#include <PlayerModel.h>
#include <VoiceManager.h>

#define RESET_STATE_DB      0
#define NEW_GAME            1
#define JOIN_GAME           2
#define LOBBY               3
#define START_GAME          4
#define SELECT_SONG         5
#define PLAY_SONG           6
#define HINT_REVEAL         7
#define PROCESS_VOICE       8
#define WAITING_ON_PLAYER_RESPONSE    9
#define CHECK_GUESS         10
#define LOCK_PLAYER         11
#define UPDATE_SCORE        12
#define SHOW_LEADERBOARD    13
#define UPDATE_ROUND        14
#define FINALIZE_WINNER     15

const int LEFT_BUTTON_PIN  = 15;
const int RIGHT_BUTTON_PIN = 2;

class GameModel {
  public:
    GameModel(U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled);

    void startNewGame(int _startTimeOfGameLobby);
    void resetStateDB();
    String  createUsername(float _yAngle, int _leftButtonFlag, int _rightButtonFlag);
    void    getPlayerUsernames();
    void    getSongIndexes();
    void    displayLobby();
    void    getHint(int _hintNumber, int _songIndex);
    void    recordUserGuess();
    void    checkPlayerGuess();
    void    updatePlayerScore();
    String  updateLeaderBoard();
    void    setUsername();
    String  leaderboard();
    void    updateRound();
    void    lockPlayer();
    void    resetLockedPlayers();
    void    getNumberOfLockedPlayers();
    bool    isSyncEnabled();
    void    syncGame(DFRobotDFPlayerMini *_myDFPlayer);
    void    resetSong();

    // state db server variables
    bool isStateDBReset();
    
    // game variables
    int  gameState();
    void setGameState(int _newGameState);
    int  playerScore();
    void setPlayerScore(int _newPlayerScore);
    bool isHintRevealed();
    void setIsHintRevealed(bool _isHintRevealed);
    bool isGuessCorrect();
    void setIsGuessCorrect(String _isGuessCorrect);
    void setIsSyncEnabled(bool _isSyncEnabled);

    // round variables
    int  roundNumber();
    void setRoundNumber(int _newRoundNumber);
    int  roundPoints();
    void setRoundPoints(int _newRoundPoints);
    int  numberOfLockedPlayers();

    // timing variables
    int  startTimeOfGameLobby();
    void setStartTimeOfGameLobby(int _startTimeOfGameLobby);
    int  startOfStartGameCountDown();
    void setStartOfStartGameCountDown(int _startOfStartGameCountDown);
    int  songStartTime();
    void setSongStartTime(int _songStartTime);
    int  hintRevealStartTime();
    void setHintRevealStartTime(int _hintRevealStartTime);
    int  lastSyncTime();
    void setLastSyncTime(int _lastSyncTime);

    String playerUsernames();
    int    currentSongIndex();
    void   setCurrentSongIndex(int _nextSongIndex);
    String hint();
    String playerGuess();

  private:
    // game state variables
    bool mIsStateDBReset;
    void setIsStateDBReset(bool _isStateDBReset);
    int  mGameState;
    bool mIsHintRevealed;
    bool mIsGuessCorrect;
    bool mIsSyncEnabled;

    // round vairables
    int  mRoundNumber;
    int  mRoundPoints;
    int  mNumberOfLockedPlayers;
    void setNumberOfLockedPlayers(int _numberOfLockedPlayers);

    // timing variables
    int  mStartTimeOfGameLobby;
    int  mStartOfStartGameCountDown;
    int  mSongStartTime;
    int  mHintRevealStartTime;
    int  mLastSyncTime;

    String  mPlayerUsernames;
    int     mSongIndexes[10];
    int     mCurrentSongIndex;
    String  mHint;
    void    setHint(String _hint);
    void    songIndexesStringToSongIndexesList(String _songIndexesString);
    
    // player variable
    PlayerModel *Player;
    WifiManager *Server;
    HelperFunctions *Helpers;
    VoiceManager    *Voice;
    U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *oled;
};

#endif