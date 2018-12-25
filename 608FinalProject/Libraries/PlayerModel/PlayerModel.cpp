#import <PlayerModel.h>

PlayerModel::PlayerModel(U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled){//U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled){
	this->mPlayerScore = 0;
	this->oled   = _oled;
	this->usernameCreator = new UsernameCreator(this->oled);//_oled);
}

String PlayerModel::createUsername(float _yAngle, int _leftButton, int _rightButton){
	return usernameCreator->update(_yAngle, _leftButton, _rightButton);
}

int PlayerModel::playerScore(){
	return mPlayerScore;
}

String PlayerModel::playerGuess(){
	return mPlayerGuess;
}

String PlayerModel::username(){
	return mUsername;
}
void PlayerModel::setPlayerGuess(String _playerGuess){
	this->mPlayerGuess = _playerGuess;
}

void PlayerModel::setPlayerScore(int _newPlayerScore){
	this->mPlayerScore = _newPlayerScore;
}

void PlayerModel::setUsername(){
	this->mUsername = this->usernameCreator->getUsername();
}