#include <GameModel.h>

GameModel::GameModel(U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled){
	this->setIsStateDBReset(false);
	this->setGameState(RESET_STATE_DB);
	this->mPlayerUsernames = "";
	this->setIsSyncEnabled(false);
    // player variable
    this->oled   = _oled;
	this->Player = new PlayerModel(this->oled);
	this->Server  = new WifiManager();
	this->Helpers = new HelperFunctions();
	this->Voice = new VoiceManager(LEFT_BUTTON_PIN ,this->oled);
}

void GameModel::startNewGame(int _startTimeOfGameLobby){
	this->setGameState(JOIN_GAME);
	this->Server->postNewState(String(this->gameState()));

	this->setLastSyncTime(millis());

	this->setIsHintRevealed(false);

    // round variables
	this->setRoundNumber(1);
	this->setRoundPoints(100);

	this->setNumberOfLockedPlayers(0);


    // timing variables
	this->setStartTimeOfGameLobby(_startTimeOfGameLobby);
}

void GameModel::resetStateDB(){
	//add Post New State function in here
	this->setIsStateDBReset(true);
	this->setGameState(NEW_GAME);
}

void GameModel::getPlayerUsernames(){
	this->mPlayerUsernames = this->Server->getOtherPlayersUserNames();
}

void GameModel::getSongIndexes(){
	String songIndexesString = this->Server->getSongIndexes();
	this->songIndexesStringToSongIndexesList(songIndexesString);
}

void GameModel::displayLobby(){
	this->oled->drawFrame(0, 0, 128, 64);
	this->Helpers->prettyPrint(45, 10, "Players" , 6, 11, 0, *this->oled);
	this->oled->drawHLine(15, 13, 96);

	String user_1 = Helpers->parse_username(this->mPlayerUsernames, 1);
	String user_2 = Helpers->parse_username(this->mPlayerUsernames, 2);
	String user_3 = Helpers->parse_username(this->mPlayerUsernames, 3);
	String user_4 = Helpers->parse_username(this->mPlayerUsernames, 4);
	String user_5 = Helpers->parse_username(this->mPlayerUsernames, 5);
	String user_6 = Helpers->parse_username(this->mPlayerUsernames, 6);

	this->Helpers->prettyPrint(25, 24, user_1, 6, 11, 0, *this->oled);
	this->Helpers->prettyPrint(25, 35, user_2, 6, 11, 0, *this->oled);
	this->Helpers->prettyPrint(25, 46, user_3, 6, 11, 0, *this->oled);
	this->Helpers->prettyPrint(80, 24, user_4, 6, 11, 0, *this->oled);
	this->Helpers->prettyPrint(80, 35, user_5, 6, 11, 0, *this->oled);
	this->Helpers->prettyPrint(80, 46, user_6, 6, 11, 0, *this->oled);

	this->oled->drawFrame(4, 48, 56, 11);
	this->oled->drawFrame(64, 48, 60, 11);
	this->Helpers->prettyPrint(8, 56, "Start Game", 6, 11, 0, *this->oled);
	this->Helpers->prettyPrint(67, 56, "Start Game", 6, 11, 0, *this->oled);
}

String GameModel::createUsername(float _yAngle, int _leftButtonFlag, int _rightButtonFlag){
	return this->Player->createUsername(_yAngle, _leftButtonFlag, _rightButtonFlag);
}

void GameModel::getHint(int _hintNumber, int _songIndex){
	String hint = this->Server->getHint(_hintNumber, _songIndex);
	this->setHint(hint); 
}

void GameModel::recordUserGuess(){
	String userGuess = this->Voice->recordUserGuess();
	this->Player->setPlayerGuess(userGuess);
}

void GameModel::checkPlayerGuess(){
	String isGuessCorrect = this->Server->checkPlayerGuess(this->currentSongIndex(), this->playerGuess());
	this->setIsGuessCorrect(isGuessCorrect);
}

void GameModel::updatePlayerScore(){
	int newPlayerScore = this->roundPoints() + this->Player->playerScore();
	this->Player->setPlayerScore(newPlayerScore);
}

void GameModel::setPlayerScore(int _newPlayerScore){
	this->Player->setPlayerScore(0);
}

String GameModel::updateLeaderBoard(){
	return this->Server->updateLeaderBoard(this->Player->username(), this->Player->playerScore());
}

void GameModel::setUsername(){
	this->Player->setUsername();
}

String GameModel::leaderboard(){
	return Server->getLeaderBoard();
}

void GameModel::updateRound(){
	this->mRoundNumber = this->mRoundNumber + 1;
	this->mRoundPoints = this->mRoundPoints * 2;
	this->setCurrentSongIndex(this->roundNumber() - 1);
	this->setIsHintRevealed(false);
	this->setIsGuessCorrect("FALSE");
}

void GameModel::lockPlayer(){
	int newPlayerScore = this->Player->playerScore() - this->roundPoints();
	this->Player->setPlayerScore(newPlayerScore);
	this->Server->adjustLockedPlayers("newlock");
}

void GameModel::resetLockedPlayers(){
	this->setNumberOfLockedPlayers(0);
	this->Server->adjustLockedPlayers("reset");
}

void GameModel::getNumberOfLockedPlayers(){
	String numberOfLockedPlayers = this->Server->getNumberOfLockedPlayers();
	this->setNumberOfLockedPlayers(numberOfLockedPlayers.toInt());
}

bool GameModel::isSyncEnabled(){
	return mIsSyncEnabled;
}

bool GameModel::isStateDBReset(){
	return mIsStateDBReset;
}

int  GameModel::gameState(){
	return mGameState;
}

int  GameModel::playerScore(){
	return Player->playerScore();
}

bool GameModel::isHintRevealed(){
	return mIsHintRevealed;
}

int GameModel::lastSyncTime(){
	return mLastSyncTime;
}

bool GameModel::isGuessCorrect(){
	return mIsGuessCorrect;
}

int  GameModel::roundNumber(){
	return mRoundNumber;
}

int  GameModel::roundPoints(){
	return mRoundPoints;
}

int  GameModel::numberOfLockedPlayers(){
	return mNumberOfLockedPlayers;
}

int  GameModel::startTimeOfGameLobby(){
	return mStartTimeOfGameLobby;
}

int GameModel::startOfStartGameCountDown(){
	return mStartOfStartGameCountDown;
}

int GameModel::songStartTime(){
	return mSongStartTime;
}

int GameModel::hintRevealStartTime(){
	return mHintRevealStartTime;
}

String GameModel::playerUsernames(){
	return mPlayerUsernames;
}

int GameModel::currentSongIndex(){
	return mCurrentSongIndex;
}

String GameModel::hint(){
	return mHint;
}

String GameModel::playerGuess(){
	return this->Player->playerGuess();
}

void GameModel::setIsStateDBReset(bool _isStateDBReset){
	this->mIsStateDBReset = _isStateDBReset;
}

void GameModel::setGameState(int _newGameState){
	this->mGameState = _newGameState;
	this->Server->postNewState(String(this->gameState()));
}

void GameModel::setRoundNumber(int _newRoundNumber){
	this->mRoundNumber = _newRoundNumber;
}

void GameModel::setRoundPoints(int _newRoundPoints){
	this->mRoundPoints = _newRoundPoints;
}

void GameModel::setNumberOfLockedPlayers(int _numberOfLockedPlayers){
	this->mNumberOfLockedPlayers = _numberOfLockedPlayers;
}

void GameModel::setStartTimeOfGameLobby(int _startTimeOfGameLobby){
	this->mStartTimeOfGameLobby = _startTimeOfGameLobby;
}

void GameModel::setStartOfStartGameCountDown(int _startOfStartGameCountDown){
	this->mStartOfStartGameCountDown = _startOfStartGameCountDown;
}

void GameModel::setSongStartTime(int _songStartTime){
	this->mSongStartTime = _songStartTime;
}

void GameModel::setHintRevealStartTime(int _hintRevealStartTime){
	this->mHintRevealStartTime = _hintRevealStartTime;
}

void GameModel::setLastSyncTime(int _lastSyncTime){
	this->mLastSyncTime = _lastSyncTime;
}

void GameModel::setIsHintRevealed(bool _isHintRevealed){
	this->mIsHintRevealed = _isHintRevealed;
}

void GameModel::setIsGuessCorrect(String _isGuessCorrect){
	if(_isGuessCorrect == "TRUE"){
		this->mIsGuessCorrect = true;
	} else if (_isGuessCorrect == "FALSE"){
		this->mIsGuessCorrect = false;
	}
}

void GameModel::setIsSyncEnabled(bool _isSyncEnabled){
	this->mIsSyncEnabled = _isSyncEnabled;
}

void GameModel::syncGame(DFRobotDFPlayerMini *_myDFPlayer){

	if(this->isSyncEnabled()){
		String latestState = this->Server->getLatestState();
		Serial.println(latestState.toInt());

		switch(latestState.toInt()){
			case START_GAME:
			{
				if(this->gameState() == LOBBY || this->gameState() == FINALIZE_WINNER){
					this->setGameState(START_GAME);
					this->setStartOfStartGameCountDown(millis());
				} else {
					// put into wait to next game state
				}
			}
			break;
			case HINT_REVEAL:
			{
				if(this->gameState() == PLAY_SONG){

					_myDFPlayer->pause();
					this->setGameState(HINT_REVEAL);
					this->getHint(1, this->currentSongIndex());
					this->setHintRevealStartTime(millis());
				}
			}
			break;
			case PROCESS_VOICE:
			{
				if(this->gameState() == PLAY_SONG || this->gameState() == HINT_REVEAL){
					Serial.println("CHANGING TO WAITING STATE");
					this->setGameState(WAITING_ON_PLAYER_RESPONSE);
					_myDFPlayer->pause();
				}
			}
			break;
			case CHECK_GUESS:
			{
				if(this->gameState() == PLAY_SONG || this->gameState() == HINT_REVEAL || this->gameState() == PROCESS_VOICE){
					Serial.println("CHANGING TO WAITING STATE");
					this->setGameState(WAITING_ON_PLAYER_RESPONSE);
					_myDFPlayer->pause();
				}
			}
			break;
			case LOCK_PLAYER:
			{
				if(this->gameState() == WAITING_ON_PLAYER_RESPONSE){
					this->setGameState(PLAY_SONG);
					this->setSongStartTime(millis());
					_myDFPlayer->start();
				}
			}
			break;
			case SHOW_LEADERBOARD:
			{
				if(this->gameState() == WAITING_ON_PLAYER_RESPONSE){
					this->setGameState(SHOW_LEADERBOARD);
				}
			}
			break;
			case UPDATE_ROUND:
			{
				if(this->gameState() == WAITING_ON_PLAYER_RESPONSE || this->gameState() == LOCK_PLAYER || this->gameState() == SHOW_LEADERBOARD){
					this->setGameState(UPDATE_ROUND);
				}
			}
			break;
			case UPDATE_SCORE:
			{
				if(this->gameState() == CHECK_GUESS || this->gameState() == PROCESS_VOICE || this->gameState() == HINT_REVEAL || this->gameState() == PLAY_SONG || this->gameState() == WAITING_ON_PLAYER_RESPONSE){
					this->setGameState(SHOW_LEADERBOARD);
				}
			}
			break;
			case WAITING_ON_PLAYER_RESPONSE:
			{
				//add pause music
				if(this->gameState() == HINT_REVEAL || this->gameState() == PLAY_SONG){
					_myDFPlayer->pause();
					this->setGameState(WAITING_ON_PLAYER_RESPONSE);
				}
			}
			break;
			case PLAY_SONG:
			{
				if(this->gameState() == WAITING_ON_PLAYER_RESPONSE){
					this->setGameState(PLAY_SONG);
					this->setSongStartTime(millis());
					_myDFPlayer->start();
				}
			}
			break;
		}
	}

	this->setLastSyncTime(millis());
}

void GameModel::resetSong(){
	this->Server->resetSongs();
}

void GameModel::setCurrentSongIndex(int _nextSongIndex){
	this->mCurrentSongIndex = this->mSongIndexes[_nextSongIndex];
}

void GameModel::setHint(String _hint){
	this->mHint = _hint;
}

void GameModel::songIndexesStringToSongIndexesList(String _songIndexesString){
	int commaCounter=0;
	for (int i=0; i<_songIndexesString.length(); i++){
		if (_songIndexesString.substring(i,i+1) == ",") {
			commaCounter += 1;
			this->mSongIndexes[commaCounter-1]=(_songIndexesString.substring(i+1, _songIndexesString.indexOf(",", i+1))).toInt();
		}
	}
}