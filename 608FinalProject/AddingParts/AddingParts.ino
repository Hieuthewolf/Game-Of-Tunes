#include <GameModel.h>
#include <mpu9255_esp32.h>

MPU9255 imu;
SCREEN oled(U8G2_R0, 5, 17, 16);
HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

//
//int timeToJoinGame;
//int songPlayingTimer;
//int hintRevealTimer;
//
//int numberOfPlayers;
//int hintCounter = 0;
//int playersLocked;
//int button_pressed;
//int lockedPlayers = 0;
//int playerScore;
//int lastStateSync = 0;
//
//String songTitle;
//bool isSyncEnable = true;
//bool isNewGame = true;
////String response;

//String userGuess;

//const unsigned response_timeout = 6000;
String leaderboardFinal;
class Button {
  public:
    unsigned long t_of_state_2;
    unsigned long t_of_button_change;
    unsigned long debounce_time;
    unsigned long long_press_time;
    int pin;
    int flag;
    bool button_pressed;
    int state; // This is public for the sake of convenience
    Button(int p) {
      flag = 0;
      state = 0;
      pin = p;
      t_of_state_2 = millis(); //init
      t_of_button_change = millis(); //init
      debounce_time = 10;
      long_press_time = 1000;
      button_pressed = 0;
    }
    void read() {
      int button_state = digitalRead(pin);
      button_pressed = !button_state;
    }
    int update() {
      read();
      flag = 0;
      switch (state) {
        case 0:
          if (button_pressed == 1) {
            state = 1;
            t_of_button_change = millis();
          }
          break;
        case 1:
          if (button_pressed == 1 && millis() - t_of_button_change > debounce_time) {
            state = 2;
            t_of_state_2 = millis();
          } else if (button_pressed == 0) {
            state = 0;
            t_of_button_change = millis();
          }
          break;
        case 2:
          if (button_pressed == 1 && millis() - t_of_state_2 > long_press_time) {
            state = 3;
          } else if (button_pressed == 0) {
            state = 4;
            t_of_button_change = millis();
          }
          break;
        case 3:
          if (button_pressed == 0) {
            state = 4;
            t_of_button_change = millis();
          }
          break;
        case 4:
          if (button_pressed == 0 && millis() - t_of_button_change >= debounce_time) {
            state = 0;
            if (millis() - t_of_state_2 <= long_press_time) {
              flag = 1;
            } else if (millis() - t_of_state_2 > long_press_time) {
              flag = 2;
            }
          } else if (button_pressed == 1 && millis() - t_of_state_2 <= long_press_time) {
            state = 2;
            t_of_button_change = millis();
          } else if (button_pressed == 1 && millis() - t_of_state_2 > long_press_time) {
            state = 3;
            t_of_button_change = millis();
          }
          break;

      }
      return flag;
    }
};

Button *LeftButton  = new Button(LEFT_BUTTON_PIN);
Button *RightButton = new Button(RIGHT_BUTTON_PIN);
GameModel *Game = new GameModel(&oled);
HelperFunctions *Helpers = new HelperFunctions();

//initialize IMU
void setupIMU() {
  while (imu.readByte(MPU9255_ADDRESS, WHO_AM_I_MPU9255) != 0x73) {
    Serial.println("NOT FOUND");
  }
  imu.initMPU9255();
  imu.getAres(); //call this so the IMU internally knows its range/resolution
}

//used to get x,y values from IMU accelerometer!
void getAngle(float& x, float& y) {
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  //voiceManager.getSpeechData().reserve(PREFIX.length() + ENC_LEN + SUFFIX.length());
  WiFi.begin("6s08", "iesc6s08"); //attempt to connect to wifi
  int count = 0; //count used for Wifi check times
  while (WiFi.status() != WL_CONNECTED && count < 6) {
    delay(1000);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.print("Connected to ");
    Serial.println("6s08");
    delay(500);
  } else { //if we failed to connect just ry again.
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP
  }

  oled.begin();     // initialize the OLED
  oled.clearBuffer();    //clear the screen contents
  oled.setFont(u8g2_font_5x7_tf);  //set font on oled
  oled.setCursor(0, 15); //set cursor
  oled.print("Starting"); //print starting message
  oled.sendBuffer(); //send it (so it displays)
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  setupIMU();
  setUpMusicReader();
  //  lastStateSync = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  int leftButtonFlag  = LeftButton->update();
  int rightButtonFlag = RightButton->update();

  if (millis() - Game->lastSyncTime() > 500 && Game->isSyncEnabled()) {
    Game->syncGame(&myDFPlayer);
  }
  //  if (millis() - lastStateSync > 500 && isSyncEnable) {

  //    String strStateUpdate = GetLatestState();
  //    int stateUpdate = strStateUpdate.toInt();
  //    if (stateUpdate == PROCESS_VOICE) {
  //      gameState = WAITING_OTHER_PLAYER_RESPONSE;
  //    } else if (stateUpdate == LOCK_PLAYER) {
  //      gameState = PLAY_SONGS;
  //    } else if (stateUpdate == UPDATE_SCORE) {
  //      gameState = UPDATE_ROUND;
  //    } else if (stateUpdate == CHECK_GUESS ) {
  //      gameState = WAITING_OTHER_PLAYER_RESPONSE;
  //    } else {
  //      gameState = stateUpdate;
  //      timeToJoinGame = millis();
  //      songPlayingTimer = millis();
  //      hintRevealTimer = millis();
  //    }
  //    lastStateSync = millis();
  //  }
  //Serial.println(Game->gameState());


  switch (Game->gameState()) {
    case RESET_STATE_DB:
      {
        if (!Game->isStateDBReset()) {
          Game->resetStateDB();
        }
      }
      break;
    case NEW_GAME:
      {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_5x7_tf);  //set font on oled
        Helpers->prettyPrint(4, 12, "Welcome: Game of Tunes", 6, 11, 0, oled);
        Helpers->prettyPrint(4, 24, "Press Right to Continue", 6, 11, 0, oled);
        oled.sendBuffer();

        if (rightButtonFlag == 1) {

          Game->startNewGame(millis());

        }
      }
      break;
    case JOIN_GAME:
      {
        // TODO - advance if 1: 5 players in lobby 2: a player as been is the lobby for 35 seconds
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        float x, y;
        getAngle(x, y);
        String currentUsername = Game->createUsername(y, leftButtonFlag, rightButtonFlag);

        oled.setFont(u8g2_font_5x7_tf);  //set font on oled
        Helpers->prettyPrint(8, 31, currentUsername, 3, 11, 0, oled);
        oled.sendBuffer();
        //              numberOfPlayers = 0;
        //              hintCounter = 0;
        //              playersLocked = 0;
        if (leftButtonFlag == 2) {

          Game->setUsername();
          Game->setGameState(LOBBY);
          Game->getPlayerUsernames();
          Game->setIsSyncEnabled(true);
          Game->setStartTimeOfGameLobby(millis());

        }
        //function to increment number of players: incrementPlayerCount
        //        if (((millis() - timeToJoinGame) > 5000) || (numberOfPlayers == 5)) {
        //          gameState = SELECT_SONGS;
        //          PostNewState(String(gameState));
        //        }
      }
      break;
    case  LOBBY:
      {
        // updates usernames probably will leaver here
        if (millis() - Game->startTimeOfGameLobby() > 2000) {
          Game->getPlayerUsernames();
          Game->setStartTimeOfGameLobby(millis());
        }

        oled.setFont(u8g2_font_5x7_tf);
        oled.clearBuffer();

        Game->displayLobby();

        oled.sendBuffer();

        if (Game->playerUsernames() == "success" || leftButtonFlag == 1) {
          Game->setGameState(START_GAME);
          Game->setStartOfStartGameCountDown(millis());
        }
      }
      break;
    case START_GAME:
      {
        oled.clearBuffer();

        if (millis() - Game->startOfStartGameCountDown() <= 15000) {

          Helpers->oled_print(String(15 - (millis() - Game->startOfStartGameCountDown()) / 1000), 53, 46, u8g2_font_timB24_tf, oled);

        } else {

          Game->getSongIndexes();
          Game->setCurrentSongIndex(Game->roundNumber() - 1);
          Game->setGameState(SELECT_SONG);
          Game->setSongStartTime(millis());

        }

        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_5x7_tf);
        Helpers->prettyPrint(10, 12, "Starting Shortly in...", 6, 11, 0, oled);
        oled.sendBuffer();
      }
      break;
    case SELECT_SONG:
      {
        myDFPlayer.play(Game->currentSongIndex());
        Game->setGameState(PLAY_SONG);
      }
      break;
    case PLAY_SONG:
      { // play song code
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        Helpers->oled_print("Points:" + String(Game->playerScore()), 5, 10, u8g2_font_profont11_mf, oled);
        Helpers->oled_print("Round:" + String(Game->roundNumber()), 85, 10, u8g2_font_profont11_mf, oled);
        // TODO handle mutiple hints
        if ((millis() - Game->songStartTime() <= 10000) && !Game->isHintRevealed()) {

          Helpers->oled_print("Hint shown in", 5, 60, u8g2_font_profont11_mf, oled);
          Helpers->oled_print(String(10 - (millis() - Game->songStartTime()) / 1000), 88, 60, u8g2_font_timB24_tf, oled);


        } else if (millis() - Game->songStartTime() <= 10000 && Game->isHintRevealed()) {

          Helpers->oled_print("Round end in", 5, 60, u8g2_font_profont11_mf, oled);
          Helpers->oled_print(String(10 - (millis() - Game->songStartTime()) / 1000), 88, 60, u8g2_font_timB24_tf, oled);

        }

        oled.setFont(u8g2_font_profont11_mf);

        if ((millis() - Game->songStartTime()) > 10000 && !Game->isHintRevealed()) {

          myDFPlayer.pause();
          Game->setGameState(HINT_REVEAL);
          Game->getHint(1, Game->currentSongIndex());
          Game->setHintRevealStartTime(millis());

        } else if (millis() - Game->songStartTime() > 10000 && Game->isHintRevealed()) {
          //playerScore -= roundPoints;
          //leaderboard_post(username_message, playerScore);
          //gameState = LOCK_PLAYER;
          Game->setGameState(LOCK_PLAYER);

        }
        if (leftButtonFlag == 1) { //adjust to register which player pressed the button

          Game->syncGame(&myDFPlayer);
          if (Game->gameState() == WAITING_ON_PLAYER_RESPONSE) {
            myDFPlayer.pause();
            Game->setGameState(WAITING_ON_PLAYER_RESPONSE);
          }
          myDFPlayer.pause();
          //Game->setIsSyncEnabled(f);
          Game->setGameState(PROCESS_VOICE);

        }

        oled.sendBuffer();
      }
      break;
    case HINT_REVEAL:
      {
        oled.clearBuffer();
        oled.setCursor(0, 15);
        //oled.print(hint);
        Helpers->oled_print("The Hint is:", 0, 10, u8g2_font_profont11_mf, oled);
        Helpers->prettyPrint(0, 25, Game->hint(), 7, 11, 0, oled);
        oled.sendBuffer();
        //hintCounter += 1;
        //reveal hint function: hintReveal();
        // TODO add handling for mutiple hints
        if (millis() - Game->hintRevealStartTime() > 4000) {

          Game->setIsHintRevealed(true);
          Game->setGameState(PLAY_SONG);
          Game->setSongStartTime(millis());
          myDFPlayer.start();

        }
      }
      break;
    case WAITING_ON_PLAYER_RESPONSE:
      {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_unifont_t_symbols);
        Helpers->oled_print("Waiting on Other Player to answer", 10, 35, u8g2_font_t0_16b_tf, oled);
        oled.sendBuffer();
      }
      break;
    case PROCESS_VOICE:
      {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_unifont_t_symbols);
        oled.drawGlyph(25, 59, 0x21e9);
        Helpers->oled_print("Hold to Answer", 10, 35, u8g2_font_t0_16b_tf, oled);
        oled.sendBuffer();

        Game->recordUserGuess();

        if (Game->playerGuess().length() != 0) {

          Game->checkPlayerGuess();
          Game->setGameState(CHECK_GUESS);

        }
      }
      break;
    case CHECK_GUESS:
      {
        oled.clearBuffer();    //clear the screen contents
        oled.drawFrame(0, 0, 128, 64);

        if (Game->isGuessCorrect()) {

          Helpers->oled_print("Correct!", 12, 30, u8g2_font_crox3cb_tr, oled);
          oled.sendBuffer();
          delay(1000);
          Game->setGameState(UPDATE_SCORE);
          Game->updateLeaderBoard();
          Game->setIsSyncEnabled(true);

        } else if (!Game->isGuessCorrect()) {

          Helpers->oled_print("Incorrect", 5, 30, u8g2_font_crox3cb_tr, oled);
          Helpers->oled_print("You can no longer answer", 10, 45, u8g2_font_haxrcorp4089_t_cyrillic, oled);
          oled.sendBuffer();
          delay(1000);

          // TODO add functionality to lock player
          Game->lockPlayer();
          Game->setGameState(LOCK_PLAYER);
          Game->setIsSyncEnabled(true);
          Game->updateLeaderBoard();

        }
      }
      break;
    case LOCK_PLAYER:
      {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        //function that locks player x from current round, lockPlayer()

        // TODO Move this under the syncing stuff so dont make a request every second
        Game->getNumberOfLockedPlayers();

        Game->numberOfLockedPlayers();
        Serial.println(Game->numberOfLockedPlayers());

        if (Game->numberOfLockedPlayers() == 5) {

          Game->resetLockedPlayers();
          Game->setGameState(SHOW_LEADERBOARD);
          //isSyncEnable = true;
        }
        Game->updateLeaderBoard();
      }
      break;
    case UPDATE_SCORE:
      {
        Game->updatePlayerScore();
        Game->setGameState(SHOW_LEADERBOARD);
        delay(500);
      }
      break;
    case SHOW_LEADERBOARD:
      {
        //TODO - change updateLeaderBoard to leaderboard and change leaderboard to clearLeaderBoard
        String leaderboard = Game->updateLeaderBoard();
        //String leaderboard = Game->leaderboard();
        Serial.println(leaderboard);
        oled.clearBuffer();
        Helpers->oled_print("Scoreboard", 25, 10, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboard, 1), 20, 20, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboard, 2), 20, 30, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboard, 3), 20, 40, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboard, 4), 20, 50, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboard, 5), 20, 60, u8g2_font_victoriabold8_8r, oled);
        oled.sendBuffer();
        delay(2000);
        Game->setGameState(UPDATE_ROUND);
      }
      break;
    case UPDATE_ROUND:
      {

        //playersLocked = 0;
        //hintCounter = 0;
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);

        Game->updateRound();

        if (Game->roundNumber() < 11) {


          Helpers->oled_print("Round " + String(Game->roundNumber()), 20, 35, u8g2_font_crox3cb_tr, oled);
          oled.sendBuffer();
          delay(2000);
          Game->resetLockedPlayers();
          Game->setGameState(SELECT_SONG);
          Game->setSongStartTime(millis());

        } else {

          leaderboardFinal = Game->leaderboard();
          Game->setGameState(FINALIZE_WINNER);

        }
      }
      break;
    case FINALIZE_WINNER:
      {
        oled.clearBuffer();
        Helpers->oled_print("Scoreboard", 25, 10, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboardFinal, 1), 20, 20, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboardFinal, 2), 20, 30, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboardFinal, 3), 20, 40, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboardFinal, 4), 20, 50, u8g2_font_victoriabold8_8r, oled);
        Helpers->oled_print( Helpers->parse_username(leaderboardFinal, 5), 20, 60, u8g2_font_victoriabold8_8r, oled);
        oled.sendBuffer();
        //function to display winner points, etc. showWinner()
        if (leftButtonFlag == 1) {
          Game->resetSong();
          Game->setRoundNumber(1);
          Game->setRoundPoints(100);
          Game->resetLockedPlayers();
          Game->setGameState(START_GAME);
        }
      }
      break;
  }
}


//String GetLatestState() {
//    WiFiClient client; //instantiate a client object
//    String response;
//    if (client.connect("iesc-s1.mit.edu", 80)) {
//      // This will send the request to the server
//      // If connected, fire off HTTP GET:
//      String thing = "";
//      client.println("GET /608dev/sandbox/ekoto4/StateDatabaseManager.py HTTP/1.1");
//      client.println("Host: iesc-s1.mit.edu");//iesc-s1.mit.edu");
//      client.println("\r\n");
//      unsigned long count = millis();
//      while (client.connected()) { //while we remain connected read out data coming back
//        String line = client.readStringUntil('\n');
//        Serial.println(line);
//        if (line == "\r") { //found a blank line!
//          //headers have been received! (indicated by blank line)
//          break;
//        }
//        if (millis() - count > response_timeout) break;
//      }
//      count = millis();
//      while (client.available()) { //read out remaining text (body of response)
//        response += (char)client.read();
//      }
//      Serial.println(response);
//      client.stop();
//      Serial.println();
//      Serial.println("-----------");
//    } else {
//      Serial.println("connection failed");
//      Serial.println("wait 0.5 sec...");
//      client.stop();
//    }
//
//    return response;
//}

void setUpMusicReader() {
  mySoftwareSerial.begin(9600, SERIAL_8N1, 32, 33);  // speed, type, RX, TX
  Serial.begin(115200);

  oled.begin();     // initialize the OLED
  oled.clearBuffer();    //clear the screen contents
  oled.setFont(u8g2_font_5x7_tf);  //set font on oled
  oled.setCursor(0, 15); //set cursor
  oled.print("Starting"); //print starting message
  oled.sendBuffer(); //send it (so it displays)

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.

    Serial.println(myDFPlayer.readType(), HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(25);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  int delayms = 100;

  //  //----Read imformation----
  //  Serial.println(F("readVolume--------------------"));
  //  Serial.println(myDFPlayer.readVolume()); //read current volume
  //  Serial.println(F("readFileCountsInFolder--------------------"));
  //  Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
  //  Serial.println(F("--------------------"));
}
