#include <U8g2lib.h>
#include <VoiceManager.h>
#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#define NEW_GAME            0
#define JOIN_GAME           1
#define SELECT_SONGS        2
#define PLAY_SONGS          3
#define HINT_REVEAL         4
#define PROCESS_VOICE       5
#define LOCK_PLAYER         6
#define UPDATE_SCORE        7
#define UPDATE_ROUND        8
#define FINALIZE_WINNER     9
#define CHECK_GUESS        10


SCREEN oled(U8G2_R0, 5, 17, 16);

const int BUTTON_PIN = 15;

int gameState = 0;
int Round;
int roundPoints;

int timeToJoinGame;
int songPlayingTimer;
int hintRevealTimer;

int numberOfPlayers;
int hintCounter = 0;
int playersLocked;
int button_pressed;
int lockedPlayers = 0;
int playerScore;
bool hint_given = false;
String songTitle;
//String response;

String userGuess;

const unsigned response_timeout = 6000;

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
VoiceManager voiceManager(BUTTON_PIN, &oled);


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
    Serial.println("MIT GUESS");
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
}

void loop() {
  // put your main code here, to run repeatedly:
  int flag = button.update();
  oled.clearBuffer();
  oled.drawFrame(0,0,128,64);
  switch (gameState) {
    case NEW_GAME:
      {
        
        oled.setCursor(0, 15);
        oled.print("New Game State");

        if (flag == 1) {
          Round = 1;
          roundPoints = 100;
          gameState = JOIN_GAME;
          timeToJoinGame = millis();
          playerScore = 0;
        }
      }
      break;
    case JOIN_GAME:
      {
        oled.setCursor(0, 15);
        oled.print("Join Game State");
        numberOfPlayers = 0;
        hintCounter = 0;
        playersLocked = 0;
        //Get the shards all together with everyone pressing the button
        //Repeat the state? until this state passes with everybody pressing the button
        //GET_POST(); //Get the post, if the message appears with Success, then go on to the next stage
        if (flag == 1) {
          //function to add plaer to the game on server
          gameState = SELECT_SONGS;
        }
        else {
          gameState = JOIN_GAME;
        }
        //function to increment number of players: incrementPlayerCount
        if (((millis() - timeToJoinGame) > 5000) || (numberOfPlayers == 5)) {
          gameState = SELECT_SONGS;
        }
      }
      break;

    //Select songs on Audrey's SD card
    case SELECT_SONGS:
      {
        //user wouldn't see this state, would be in the background
        // function to randomly select songs: pickSong();, returns song and songTitle
        gameState = PLAY_SONGS;
        songPlayingTimer = millis();

      }
      break;

    //Play the songs  and if somebody presses button to answer, then the song will stop playing
    case PLAY_SONGS:
      {
        int button_state = digitalRead(BUTTON_PIN);
        oled_print("Points:" + String(playerScore),5,10,u8g2_font_profont11_mf);
        oled_print("Round:" + String(Round),85,10,u8g2_font_profont11_mf);
        if (millis() - songPlayingTimer <= 10000 && !hint_given){
          oled_print("Hint shown in",8,55,u8g2_font_profont11_mf);
          oled_print(String(10-(millis()-songPlayingTimer)/1000),88,55,u8g2_font_timB24_tf);
        }
        else if (millis() - songPlayingTimer <= 10000 && hint_given){
          oled_print("Round end in",8,55,u8g2_font_profont11_mf);
          oled_print(String(10-(millis()-songPlayingTimer)/1000),88,55,u8g2_font_timB24_tf);
        }
        oled.setFont(u8g2_font_profont11_mf);
        if ((millis() - songPlayingTimer) > 10000) {
          hintRevealTimer = millis();
          gameState = HINT_REVEAL;
        }
        if (flag==1) { //adjust to register which player pressed the button
          gameState = PROCESS_VOICE;
        }
        if (millis()- songPlayingTimer >10000 && hint_given){
          playerScore -= roundPoints;
          gameState=LOCK_PLAYER;
        }
      }
      break;

    case HINT_REVEAL:
      {
        oled_print("The Hint is", 20,20,u8g2_font_helvB12_tf);
        oled_print("'Artist Name'", 15,45,u8g2_font_helvB12_tf);
        hintCounter += 1;
        //reveal hint function: hintReveal();
        if (millis() - hintRevealTimer > 4000 || flag == 1) {
          songPlayingTimer = millis();
          hint_given=true;
          gameState = PLAY_SONGS;
        }
      }
      break;

    //Using Hieu's and David's voice program, we will set the name of the songs to be predetermined
    //In this function, if the server side script returns True, then update the score, if it's false then locks the player from the round
    case PROCESS_VOICE:
      {
        oled.drawFrame(0,0,128,64);
        oled.setFont(u8g2_font_unifont_t_symbols);
        oled.drawGlyph(25,59,0x21e9);
        oled_print("Hold to Answer", 10, 35,u8g2_font_t0_16b_tf  );
        //function to process song and read words said as title of song: readResponse(), returns response string
        voiceManager.recordUserGuess();
        userGuess = voiceManager.getUserGuess();
        if (userGuess.length() != 0) {
          gameState = CHECK_GUESS;
        }
      }
      break;

    case CHECK_GUESS:
      {
        Serial.println(userGuess);
        //String url = "/608dev/sandbox/ekoto4/ServerHandler.py?state=checkguess&songIndex=6&guess=" + userGuess;
        String isGuessCorrect = DoGet(userGuess);//"GET", "iesc-s1.mit.edu", url);
        userGuess = "";
        Serial.println(isGuessCorrect);
        if (isGuessCorrect == "TRUE") {
          oled_print("Correct!", 12, 30, u8g2_font_crox3cb_tr);
          oled.sendBuffer();
          delay(1000);
          gameState = UPDATE_SCORE;

        }
        else if (isGuessCorrect == "FALSE") {
          oled_print("Incorrect", 5, 30, u8g2_font_crox3cb_tr);
          oled_print("You can no longer answer", 10, 45, u8g2_font_haxrcorp4089_t_cyrillic);
          oled.sendBuffer();
          delay(1000);
          gameState = LOCK_PLAYER;
        }
      }
      break;

    case LOCK_PLAYER:
      {
        //function that locks player x from current round, lockPlayer()
        lockedPlayers += 1;
        playerScore -= roundPoints;
        oled_print ( "Your Score:" + String(playerScore), 5, 35, u8g2_font_artosserif8_8r );
        oled.sendBuffer();
        delay(2000);
        if (lockedPlayers == 1) {
          lockedPlayers=0;
          gameState = UPDATE_ROUND;
        }
        else {
          songPlayingTimer = millis();
          hint_given=false;
          gameState = PLAY_SONGS;
          //warning: we need to cover the case in which the last player(s) don't press anything and jsut waste time listening to the song, doing nothing, it's stuck how we currently have the cases
        }

      }
      break;

    case UPDATE_SCORE:
      {
        //function that updates score of player x, updateScore()
        //player wouldn't see this state, would be taking place in the background
        playerScore += roundPoints;
        userGuess = "";
        oled_print ( "Your Score:" + String(playerScore), 5, 35,u8g2_font_artosserif8_8r );
        oled.sendBuffer();
        delay(2000);
        gameState = UPDATE_ROUND;
      }
      break;

    //When updating round, update the score of the player, maybe nice interface and what not
    case UPDATE_ROUND:
      {
        playersLocked = 0;
        hintCounter = 0;
        Round += 1;
        //roundPoints *= Round;
        oled_print("Round " + String(Round),20, 35, u8g2_font_crox3cb_tr);
        oled.sendBuffer();
        delay(2000);
        if (Round < 10) {
          songPlayingTimer = millis();
          gameState = SELECT_SONGS;

        }
        else {
          gameState = FINALIZE_WINNER;

        }
      }
      break;

    //In the end, have a scoreboard of everybody who has played in that current round, each round will reset the scoreboard
    case FINALIZE_WINNER:
      {
        oled_print("Scoreboard", 25, 10, u8g2_font_victoriabold8_8r );
        oled_print("Player1: " + String(playerScore), 15, 20,u8g2_font_victoriabold8_8r );
        oled_print("Player2: " + String(playerScore), 15, 30,u8g2_font_victoriabold8_8r );
        oled_print("Player3: " + String(playerScore), 15, 40,u8g2_font_victoriabold8_8r );
        oled_print("Player4: " + String(playerScore), 15, 50,u8g2_font_victoriabold8_8r );
        oled_print("Player5: " + String(playerScore), 15, 60,u8g2_font_victoriabold8_8r );
        //function to display winner points, etc. showWinner()
        if (flag == 1) {
          gameState = NEW_GAME;
        }
      }
      break;
  }
  oled.sendBuffer();
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

void oled_print(String input,int startx,int starty, const uint8_t* font){
  oled.setFont(font);
  oled.setCursor(startx,starty);
  oled.print(input);
   // update the screen
}
//bool isSongGuessCorrect(int _songIndex, String _guess) {
//
//}
