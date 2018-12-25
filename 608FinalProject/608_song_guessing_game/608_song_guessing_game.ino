#include <GameModel.h>
//#include <U8g2lib.h>
//#include <VoiceManager.h>
#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
//#define NEW_GAME            0
//#define JOIN_GAME           1
//#define SELECT_SONGS        2
//#define PLAY_SONGS          3
//#define HINT_REVEAL         4
//#define PROCESS_VOICE       5
//#define LOCK_PLAYER         6
//#define UPDATE_SCORE        7
//#define UPDATE_ROUND        8
//#define FINALIZE_WINNER     9
//#define CHECK_GUESS        10
//#define WAITING_OTHER_PLAYER_RESPONSE  11

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 148

SCREEN oled(U8G2_R0, 5, 17, 16);

const int BUTTON_PIN = 15;

//int gameState = 0;
//int Round;
//int roundPoints;
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

String userGuess;

const unsigned response_timeout = 6000;

void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display) {
  int q = 0;
  int count = 0;
  display.setCursor(startx,starty);
  for (q = 0; q < input.length(); q++) {
    if (count+startx+fwidth > SCREEN_WIDTH) {
      starty = starty+fheight+spacing;
      display.setCursor(startx,starty);
      count = 0;
    }
    if (input.substring(q,q+1) == "\n") {
      starty = starty+fheight+spacing;
      display.setCursor(startx,starty);
      count = 0;
    }
    if (starty > SCREEN_HEIGHT) {
      break;
    }
    count = count + fwidth;
    display.print(input.substring(q,q+1));
  }
}

//class SongGuessingGame() {
//  public:
//    void SongGuessingGame() {
//      songIndex
//    }
//
//    void UpdateGame() {
//    }
//
//  private:
//    int songIndex;
//}

//class Player() {
//  public:
//    void UpdateScore() {
//
//    }
//}

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

Button button(BUTTON_PIN);
//VoiceManager voiceManager(BUTTON_PIN, &oled);
GameModel *Game = new GameModel(BUTTON_PIN);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  //voiceManager.getSpeechData().reserve(PREFIX.length() + ENC_LEN + SUFFIX.length());
  WiFi.begin("MIT"); //attempt to connect to wifi
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
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //  lastStateSync = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  int flag = button.update();
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
        Serial.println(gameState);
        oled.clearBuffer();
        oled.setCursor(0, 15);
        oled.print("Join Game State");
        oled.sendBuffer();
        numberOfPlayers = 0;
        hintCounter = 0;
        playersLocked = 0;
        //Get the shards all together with everyone pressing the button
        //Repeat the state? until this state passes with everybody pressing the button
        //GET_POST(); //Get the post, if the message appears with Success, then go on to the next stage
        if (flag == 1) {
          //function to add plaer to the game on server
          gameState = SELECT_SONGS;
          PostNewState(String(gameState));
        }
        else {
          gameState = JOIN_GAME;
          PostNewState(String(gameState));
        }
        //function to increment number of players: incrementPlayerCount
        if (((millis() - timeToJoinGame) > 5000) || (numberOfPlayers == 5)) {
          gameState = SELECT_SONGS;
          PostNewState(String(gameState));
        }
      }
      break;

    //Select songs on Audrey's SD card
    case SELECT_SONGS:
      {
        oled.clearBuffer();
        oled.setCursor(0, 15);
        oled.print("Select Songs State");
        oled.sendBuffer();
        //user wouldn't see this state, would be in the background
        delay(500);

        // function to randomly select songs: pickSong();, returns song and songTitle
        gameState = PLAY_SONGS;
        PostNewState(String(gameState));
        songPlayingTimer = millis();

      }
      break;

    //Play the songs  and if somebody presses button to answer, then the song will stop playing
    case PLAY_SONGS:
      {
        oled.clearBuffer();
        oled.setCursor(0, 15);
        oled.print("Play Songs State");
        oled.sendBuffer();
        if ((millis() - songPlayingTimer) > 10000) {
          hintRevealTimer = millis();
          hintCounter += 1;
          if (hintCounter < 3) {
            gameState = HINT_REVEAL;
          } else {
            roundPoints = -100;
            gameState = UPDATE_SCORE;
          }
          PostNewState(String(gameState));
          isSyncEnable = false;
        }
        if (flag == 1) { //adjust to register which player pressed the button
          gameState = PROCESS_VOICE;
          PostNewState(String(gameState));
          isSyncEnable = false;
        }
      }
      break;

    case HINT_REVEAL:
      {
        
        String hint = GetHint(hintCounter);
        oled.clearBuffer();
        oled.setCursor(0, 15);
        //oled.print(hint);
        oled_print("The Hint is:", 0, 10, u8g2_font_profont11_mf);
        pretty_print(0, 25, hint, 7, 11, 0, oled);
        oled.sendBuffer();
        Serial.println(hint);
        //reveal hint function: hintReveal();
        if (millis() - hintRevealTimer > 5000) {
          songPlayingTimer = millis();
          Game->gameState() = PLAY_SONGS;
          PostNewState(String(gameState));
        }
      }
      break;
    case NEW_GAME:
      {
        //        if (isNewGame) {
        //          PostNewState(String(gameState));
        //          isNewGame = false;
        //        }
        oled.clearBuffer();
        oled.setCursor(0, 15);
        oled.print("New Game State");
        oled.sendBuffer();
        if (flag == 1) {
          Game->startNewGame(millis());
          Serial.println(Game->roundNumber());
          Serial.println(Game->gameState());
          Serial.println(Game->startTimeOfGameLobby());
          //          Round = 1;
          //          roundPoints = 100;
          //          gameState = JOIN_GAME;
          //          timeToJoinGame = millis();
          //          playerScore = 0;
          //          PostNewState(String(gameState));
          // post state advancement
        }
      }
      break;
//          case JOIN_GAME:
//            {
//              oled.clearBuffer();
//              oled.setCursor(0, 15);
//              oled.print("Join Game State");
//              oled.sendBuffer();
////              numberOfPlayers = 0;
////              hintCounter = 0;
////              playersLocked = 0;
//              //Get the shards all together with everyone pressing the button
//              //Repeat the state? until this state passes with everybody pressing the button
//              //GET_POST(); //Get the post, if the message appears with Success, then go on to the next stage
//              if (flag == 1) {
//                //function to add plaer to the game on server
//                gameState = SELECT_SONGS;
//                PostNewState(String(gameState));
//              } else {
//                gameState = JOIN_GAME;
//                PostNewState(String(gameState));
//              }
//              //function to increment number of players: incrementPlayerCount
//              if (((millis() - timeToJoinGame) > 5000) || (numberOfPlayers == 5)) {
//                gameState = SELECT_SONGS;
//                PostNewState(String(gameState));
//              }
//            }
//            break;
      //
      //    //Select songs on Audrey's SD card
      //    case SELECT_SONGS:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Select Songs State");
      //        oled.sendBuffer();
      //        //user wouldn't see this state, would be in the background
      //        delay(500);
      //
      //        // function to randomly select songs: pickSong();, returns song and songTitle
      //        gameState = PLAY_SONGS;
      //        PostNewState(String(gameState));
      //        songPlayingTimer = millis();
      //
      //      }
      //      break;
      //
      //    //Play the songs  and if somebody presses button to answer, then the song will stop playing
      //    case PLAY_SONGS:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Play Songs State");
      //        oled.sendBuffer();
      //        if ((millis() - songPlayingTimer) > 10000) {
      //          hintRevealTimer = millis();
      //          gameState = HINT_REVEAL;
      //          PostNewState(String(gameState));
      //          isSyncEnable = false;
      //        }
      //        if (flag == 1) { //adjust to register which player pressed the button
      //          gameState = PROCESS_VOICE;
      //          PostNewState(String(gameState));
      //          isSyncEnable = false;
      //        }
      //      }
      //      break;
      //
      //    case HINT_REVEAL:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Hint Reveal State");
      //        oled.sendBuffer();
      //        hintCounter += 1;
      //        //reveal hint function: hintReveal();
      //        if (millis() - hintRevealTimer > 4000) {
      //          songPlayingTimer = millis();
      //          gameState = PLAY_SONGS;
      //          PostNewState(String(gameState));
      //        }
      //      }
      //      break;
      //
      //    //Using Hieu's and David's voice program, we will set the name of the songs to be predetermined
      //    //In this function, if the server side script returns True, then update the score, if it's false then locks the player from the round
      //    case PROCESS_VOICE:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Process Voice State");
      //        oled.sendBuffer();
      //        //function to process song and read words said as title of song: readResponse(), returns response string
      //        voiceManager.recordUserGuess();
      //        userGuess = voiceManager.getUserGuess();
      //        if (userGuess.length() != 0) {
      //          gameState = CHECK_GUESS;
      //          PostNewState(String(gameState));
      //        }
      //      }
      //      break;
      //
      //    case CHECK_GUESS:
      //      {
      //        Serial.println(userGuess);
      //        //String url = "/608dev/sandbox/ekoto4/ServerHandler.py?state=checkguess&songIndex=6&guess=" + userGuess;
      //        String isGuessCorrect = DoGet(userGuess);//"GET", "iesc-s1.mit.edu", url);
      //        userGuess = "";
      //        oled.clearBuffer();    //clear the screen contents
      //        oled.setCursor(0, 15);
      //        oled.print(isGuessCorrect);
      //        oled.sendBuffer();     // update the screen
      //        Serial.println(isGuessCorrect);
      //        delay(1000);
      //        if (isGuessCorrect == "TRUE") {
      //          gameState = UPDATE_SCORE;
      //          PostNewState(String(gameState));
      //        }
      //        else if (isGuessCorrect == "FALSE") {
      //          gameState = LOCK_PLAYER;
      //          PostNewState(String(gameState));
      //        }
      //      }
      //      break;
      //
      //    case WAITING_OTHER_PLAYER_RESPONSE:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Waiting on other player");
      //        oled.sendBuffer();
      //      }
      //      break;
      //
      //    case LOCK_PLAYER:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Lock Players State");
      //        oled.sendBuffer();
      //        //function that locks player x from current round, lockPlayer()
      //        lockedPlayers += 1;
      //        playerScore -= roundPoints;
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Points: " + String(playerScore));
      //        oled.sendBuffer();
      //        delay(2000);
      //        if (lockedPlayers == 5) {
      //          gameState = UPDATE_ROUND;
      //          PostNewState(String(gameState));
      //          isSyncEnable = true;
      //        }
      //        else {
      //          songPlayingTimer = millis();
      //          gameState = PLAY_SONGS;
      //          PostNewState(String(gameState));
      //          isSyncEnable = true;
      //          //warning: we need to cover the case in which the last player(s) don't press anything and jsut waste time listening to the song, doing nothing, it's stuck how we currently have the cases
      //        }
      //
      //      }
      //      break;
      //
      //    case UPDATE_SCORE:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Update Scores State");
      //        oled.sendBuffer();
      //        //function that updates score of player x, updateScore()
      //        //player wouldn't see this state, would be taking place in the background
      //        playerScore += roundPoints;
      //        userGuess = "";
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Points: " + String(playerScore));
      //        oled.sendBuffer();
      //        delay(2000);
      //        gameState = UPDATE_ROUND;
      //        PostNewState(String(gameState));
      //        isSyncEnable = true;
      //      }
      //      break;
      //
      //    //When updating round, update the score of the player, maybe nice interface and what not
      //    case UPDATE_ROUND:
      //      {
      //        playersLocked = 0;
      //        hintCounter = 0;
      //        Round += 1;
      //        roundPoints *= Round;
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Update Round State Round: " + String(Round));
      //        oled.sendBuffer();
      //        delay(2000);
      //        if (Round < 10) {
      //          songPlayingTimer = millis();
      //          gameState = SELECT_SONGS;
      //          PostNewState(String(gameState));
      //        }
      //        else {
      //          gameState = FINALIZE_WINNER;
      //          PostNewState(String(gameState));
      //        }
      //      }
      //      break;
      //
      //    //In the end, have a scoreboard of everybody who has played in that current round, each round will reset the scoreboard
      //    case FINALIZE_WINNER:
      //      {
      //        oled.clearBuffer();
      //        oled.setCursor(0, 15);
      //        oled.print("Finalize Winner State");
      //        oled.sendBuffer();
      //        //function to display winner points, etc. showWinner()
      //        if (flag == 1) {
      //          gameState = NEW_GAME;
      //          PostNewState(String(gameState));
      //        }
      //      }
      //      break;
  }
}

String DoGet(String _userGuess) { //String _method, String _host, String _url) {
  WiFiClient client; //instantiate a client object
  String response;
  if (client.connect("iesc-s1.mit.edu", 80)) {
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "";
    client.println("GET /608dev/sandbox/ekoto4/ServerHandler.py?state=checkguess&songIndex=6&guess=" + _userGuess + " HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");//iesc-s1.mit.edu");
    client.println("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line)
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      response += (char)client.read();
    }
    Serial.println(response);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }

  return response;
}

String GetHint(int _hintNum) { //String _method, String _host, String _url) {
  WiFiClient client; //instantiate a client object
  String response;
  if (client.connect("iesc-s1.mit.edu", 80)) {
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "";
    client.println("GET /608dev/sandbox/ekoto4/ServerHandler.py?state=sendHint&songIndex=6&hintNum=" + String(_hintNum) + " HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");//iesc-s1.mit.edu");
    client.println("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line)
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      response += (char)client.read();
    }
    Serial.println(response);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }

  return response;
}

//void PostNewState() {
//
//}

void PostNewState(String _gameState) {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to numbersapi.com host
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String content = "state=" + _gameState;
    client.println("POST /608dev/sandbox/ekoto4/StateDatabaseManager.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(content.length()));
    client.print("\r\n");
    client.print(content);
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line)
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    count = millis();
    String op; //create empty String object
    while (client.available()) { //read out remaining text (body of response)
      op += (char)client.read();
    }
    Serial.println(op);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
    delay(300);
  }
}

String GetLatestState() {
  WiFiClient client; //instantiate a client object
  String response;
  if (client.connect("iesc-s1.mit.edu", 80)) {
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "";
    client.println("GET /608dev/sandbox/ekoto4/StateDatabaseManager.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");//iesc-s1.mit.edu");
    client.println("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line)
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      response += (char)client.read();
    }
    Serial.println(response);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }

  return response;
}

void oled_print(String input,int startx,int starty, const uint8_t* font){
  oled.setFont(font);
  oled.setCursor(startx,starty);
  oled.print(input);
   // update the screen
}

//void DoSyncGet() { //String _method, String _host, String _url) {
//  WiFiClient client; //instantiate a client object
//  String response;
//  if (client.connect("iesc-s1.mit.edu", 80)) {
//    // This will send the request to the server
//    // If connected, fire off HTTP GET:
//    String thing = "";
//    client.println("GET /608dev/sandbox/bamlak/stateDBtest.py HTTP/1.1");
//    client.println("Host: iesc-s1.mit.edu");//iesc-s1.mit.edu");
//    client.println("\r\n");
//    unsigned long count = millis();
//    while (client.connected()) { //while we remain connected read out data coming back
//      String line = client.readStringUntil('\n');
//      Serial.println(line);
//      if (line == "\r") { //found a blank line!
//        //headers have been received! (indicated by blank line)
//        break;
//      }
//      if (millis() - count > response_timeout) break;
//    }
//    count = millis();
//    while (client.available()) { //read out remaining text (body of response)
//      response += (char)client.read();
//    }
//    Serial.println(response);
//    client.stop();
//    Serial.println();
//    Serial.println("-----------");
//  } else {
//    Serial.println("connection failed");
//    Serial.println("wait 0.5 sec...");
//    client.stop();
//  }
//
//  return response;
//}



//bool isSongGuessCorrect(int _songIndex, String _guess) {
//
//}
