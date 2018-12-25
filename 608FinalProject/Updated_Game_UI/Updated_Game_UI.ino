
#include <U8g2lib.h>
#include <VoiceManager.h>
#include <WiFi.h>
#include <mpu9255_esp32.h>

#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI
#define NEW_GAME            0
#define JOIN_GAME           1
#define LOBBY               2

//#define READY_GAME          3

#define START_GAME          3
#define SELECT_SONGS        4
#define PLAY_SONGS          5
#define HINT_REVEAL         6
#define PROCESS_VOICE       7
#define LOCK_PLAYER         8
#define UPDATE_SCORE        9
#define UPDATE_ROUND        10
#define FINALIZE_WINNER     11
#define CHECK_GUESS         12

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 148

MPU9255 imu; //imu object called, appropriately, imu

int input;
String username_post_output;
String readyplayer_post_output;
String username_get_output;
String readyplayer_get_output;
String leaderboard;

bool restart;

String user_1;
String user_2;
String user_3;
String user_4;
String user_5;
String user_6;
String username_message; //contains previous query response
String query_string = "";

SCREEN oled(U8G2_R0, 5, 17, 16);

const int BUTTON_PIN = 15;
const int BUTTON_PIN_2 = 2;

unsigned long restart_timer;
unsigned long gaming_timer;
unsigned long game_start_timer;
unsigned long lobby_timer;

int gameState = 0;
int Round = 1;
int roundPoints = 100;

int timeToJoinGame;
int songPlayingTimer;
int hintRevealTimer;
int songindex[10];
int numberOfPlayers;
int hintCounter = 0;
int playersLocked;
int button_pressed;
int lockedPlayers = 0;
int playerScore;
bool hint_given = false;
String songTitle;
String songstring;
String numberoflocks;
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

void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display) {
  int q = 0;
  int count = 0;
  display.setCursor(startx, starty);
  for (q = 0; q < input.length(); q++) {
    if (count + startx + fwidth > SCREEN_WIDTH) {
      starty = starty + fheight + spacing;
      display.setCursor(startx, starty);
      count = 0;
    }
    if (input.substring(q, q + 1) == "\n") {
      starty = starty + fheight + spacing;
      display.setCursor(startx, starty);
      count = 0;
    }
    if (starty > SCREEN_HEIGHT) {
      break;
    }
    count = count + fwidth;
    display.print(input.substring(q, q + 1));
  }
}

void username_post() {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "user=" + username_message;
    client.println("POST /608dev/sandbox/hieutan/finalproject/final_project_lobby.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(thing.length()));
    client.print("\r\n");
    client.print(thing);
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
      username_post_output += (char)client.read();
    }
    Serial.println(username_post_output);
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

void username_get() {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) {
    client.println("GET http://iesc-s1.mit.edu/608dev/sandbox/hieutan/finalproject/final_project_lobby.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.print("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back (Header portion)
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line) --> seperates the header from the body
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    username_get_output = "";
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      username_get_output += (char)client.read();
    }
    Serial.println(username_get_output);
    client.stop();
    Serial.println();
    Serial.println("----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

void leaderboard_get() {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) {
    client.println("GET http://iesc-s1.mit.edu/608dev/sandbox/daiyunli/leaderboard.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.print("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back (Header portion)
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line) --> seperates the header from the body
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    leaderboard = "";
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      leaderboard += (char)client.read();
    }
    Serial.println(readyplayer_get_output);
    client.stop();
    Serial.println();
    Serial.println("----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
void leaderboard_post(String player, int score) {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "player=" + player + "&score=" + score;
    client.println("POST /608dev/sandbox/daiyunli/leaderboard.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(thing.length()));
    client.print("\r\n");
    client.print(thing);
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
    leaderboard = "";
    while (client.available()) { //read out remaining text (body of response)
      leaderboard += (char)client.read();
    }
    Serial.println(readyplayer_post_output);
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

class UsernameGetter {
    String alphabet = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int char_index;
    int state;
    unsigned long scrolling_timer;
    int scrolling_threshold = 195;
    float angle_threshold = 0.4;
  public:
    UsernameGetter() {
      state = 0;
      username_message = "";
      char_index = 0;
      scrolling_timer = millis();
    }
    String update(float angle, int button_1, int button_2) {
      switch (state) {
        case 0:
          gaming_timer = 0;
          timeToJoinGame = millis();
          oled.setFont(u8g2_font_crox1tb_tf);
          pretty_print(20, 20, "Pick a Username!", 5, 11, 0, oled);
          pretty_print(71, 40, "Hold This", 5, 11, 0, oled);
          oled.setFont(u8g2_font_unifont_t_symbols);
          oled.drawGlyph(92, 59, 0x21e9);
          oled.drawGlyph(20, 40, 0x2655);
          oled.drawGlyph(40, 40, 9818);
          oled.drawGlyph(20, 57, 9822);
          oled.drawGlyph(40, 57, 9813);

          if (button_2 == 2) {
            timeToJoinGame = millis();
            state = 1;
            char_index = 1;
            scrolling_timer = millis();
            query_string = "";
            return username_message;
          }
          if (button_2 == 1) {
            return username_message;
          }
          return username_message;
          break;

        case 1:
          oled.setFont(u8g2_font_crox1tb_tf);
          pretty_print(4, 16, "Choose your username", 5, 11, 0, oled);
          oled.drawFrame(4, 22, 113, 12);
          oled.setFont(u8g2_font_5x7_tf);
          pretty_print(48, 45, "Hold to Confirm", 2, 11, 0, oled);
          oled.setFont(u8g2_font_unifont_t_symbols);
          oled.drawGlyph(88, 62, 0x21e9);
          oled.drawGlyph(10, 48, 0x2655);
          oled.drawGlyph(25, 48, 9818);
          oled.drawGlyph(10, 62, 9822);
          oled.drawGlyph(25, 62, 9813);
          if (button_2 == 1 && state == 1) {
            query_string += alphabet[char_index];
            char_index = 0;
            scrolling_timer = millis();
            username_message = query_string;
          }
          if (button_2 == 2 && state == 1) {
            timeToJoinGame = millis();
            state = 2;
            return "";
          }
          if (angle >= angle_threshold && millis() - scrolling_timer >= scrolling_threshold) {
            char_index += 1;
            if (char_index > alphabet.length()) {
              char_index = 0;
            }
            scrolling_timer = millis();
          }

          else if (angle <= -angle_threshold && millis() - scrolling_timer >= scrolling_threshold) {
            char_index -= 1;
            if (char_index < 0) {
              char_index = alphabet.length() - 1;
            }
            scrolling_timer = millis();
          }
          oled.setCursor(0, 50);
          return query_string + alphabet[char_index];
          break;

        case 2:
          oled.setFont(u8g2_font_unifont_t_symbols);
          oled.drawGlyph(40, 35, 9824);
          oled.drawGlyph(55, 35, 9825);
          oled.drawGlyph(70, 35, 9831);
          oled.drawGlyph(85, 35, 9830);
          oled_print("Is " + username_message + " Correct?", 4, 16, u8g2_font_crox1tb_tf);
          if (button_2 == 1) {
            query_string = "";
            state = 1;
          }
          if (button_1 == 1) {
            timeToJoinGame = millis();
            gaming_timer = millis();
            username_post();
            query_string = "";
            state = 3;
            return "";
          }
          oled.setFont(u8g2_font_unifont_t_symbols);
          oled.drawGlyph(25, 57, 0x2714);
          oled.drawGlyph(92, 57, 0x2716);
          return "";

          break;

        case 3:
          if (millis() - restart_timer > 2000 && restart == true) {
            state = 0;
            restart = false;
            username_message = "";
          }
          oled.setFont(u8g2_font_px437wyse700b_mf);
          pretty_print(4, 18, "Player:", 6, 11, 0, oled);
          pretty_print(4, 36, username_message, 6, 11, 0, oled);
          pretty_print(4, 54, "Wait..", 6, 11, 0, oled);
          return "";
          break;
      }
    }
};

UsernameGetter ug;
Button button_1(BUTTON_PIN);
Button button_2(BUTTON_PIN_2);
VoiceManager voiceManager(BUTTON_PIN, &oled);

//initialize IMU
void setup_imu() {
  while (imu.readByte(MPU9255_ADDRESS, WHO_AM_I_MPU9255) != 0x73) {
    Serial.println("NOT FOUND");
  }
  imu.initMPU9255();
  imu.getAres(); //call this so the IMU internally knows its range/resolution
}

//used to get x,y values from IMU accelerometer!
void get_angle(float& x, float& y) {
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  //voiceManager.getSpeechData().reserve(PREFIX.length() + ENC_LEN + SUFFIX.length());
  WiFi.begin("MIT GUEST"); //attempt to connect to wifi
  int count = 0; //count used for Wifi check times
  while (WiFi.status() != WL_CONNECTED && count < 6) {
    delay(1000);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.print("Connected to ");
    Serial.println("MIT GUEST");
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
  game_start_timer = millis();
  lobby_timer = millis();
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  input = 0;
  setup_imu();
  restart = false;
}

void loop() {
  // put your main code here, to run repeatedly:
  int flag_1 = button_1.update();
  int flag_2 = button_2.update();
  
  oled.clearBuffer();
  switch (gameState) {
    case NEW_GAME:
      {
        timeToJoinGame = millis();
        restart_timer = millis();
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_crox1tb_tf);
        pretty_print(2, 12, "Welcome to...", 6, 11, 0, oled);
        pretty_print(2, 26, "Game of Tunes!", 6, 11, 0, oled);
        pretty_print(2, 40, "Press Right to Continue", 6, 11, 0, oled);
        oled.setFont(u8g2_font_unifont_t_symbols);
        oled.drawGlyph(95, 20, 0x2654);
        oled.drawGlyph(105, 20, 0x2619);
        oled.sendBuffer();
        if (flag_2 == 1) {
          restart_timer = millis();
          timeToJoinGame = millis();
          Round = 1;
          roundPoints = 100;
          gameState = JOIN_GAME;
          playerScore = 0;
        }
      }
      break;
    case JOIN_GAME:
      {
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        float x, y;
        get_angle(x, y); //get angle values
        String output = ug.update(y, flag_1, flag_2); //input: angle and button, output String to display on this timestep
        oled.setFont(u8g2_font_5x7_tf);  //set font on oled
        pretty_print(8, 31, output, 3, 11, 0, oled);
        oled.sendBuffer();
        numberOfPlayers = 0;
        hintCounter = 0;
        playersLocked = 0;
        if (millis() - timeToJoinGame > 5000 && gaming_timer > 3000) {
          //function to add plaer to the game on server
          lobby_timer = millis();
          gameState = LOBBY;
        }
      }
      break;

    case LOBBY:
      {
        if (millis() - lobby_timer > 1500) {
          username_get();
          lobby_timer = millis();
        }
        oled.setFont(u8g2_font_5x7_tf);
        oled.clearBuffer();
        oled.drawFrame(0, 0, 128, 64);
        pretty_print(45, 10, "Players" , 6, 11, 0, oled);
        oled.drawHLine(15, 13, 96);
        user_1 = parse_username(username_get_output, 1);
        user_2 = parse_username(username_get_output, 2);
        user_3 = parse_username(username_get_output, 3);
        user_4 = parse_username(username_get_output, 4);
        user_5 = parse_username(username_get_output, 5);
        user_6 = parse_username(username_get_output, 6);

        pretty_print(25, 24, user_1, 6, 11, 0, oled);
        pretty_print(25, 35, user_2, 6, 11, 0, oled);
        pretty_print(25, 46, user_3, 6, 11, 0, oled);
        pretty_print(80, 24, user_4, 6, 11, 0, oled);
        pretty_print(80, 35, user_5, 6, 11, 0, oled);
        pretty_print(80, 46, user_6, 6, 11, 0, oled);


        oled.drawFrame(4, 48, 56, 11);
        pretty_print(8, 56, "Start Game", 6, 11, 0, oled);

        oled.sendBuffer();

        if (flag_1 == 1 ) {
          gameState = START_GAME;
          game_start_timer = millis();
        }

        if (username_get_output == "success") {
          gameState = START_GAME;
          game_start_timer = millis();
        }
      }

      break;

    //    case READY_GAME:
    //      {
    //        oled.clearBuffer();
    //        oled.drawFrame(0, 0, 128, 64);
    //        oled.setFont(u8g2_font_5x7_tf);
    //        pretty_print(4, 12, "Ready by Pressing!", 6, 11, 0, oled);
    //        oled.setFont(u8g2_font_unifont_t_symbols);
    //        oled.drawGlyph(92, 59, 0x21e9);
    //        oled.sendBuffer();
    //        if (flag_2 == 1) {
    //
    //          songPlayingTimer = millis();
    //        }
    //      }
    //      break;

    case START_GAME:
      {
        oled.clearBuffer();
        if (millis() - game_start_timer <= 10000) {
          oled_print(String(10 - (millis() - game_start_timer) / 1000), 53, 46, u8g2_font_timB24_tf);
        }
        else {
          songindex_get();
          songindex_listmaker(songstring);
          Serial.print(songindex[1]);
          gameState = SELECT_SONGS;
        }
        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_5x7_tf);
        pretty_print(10, 12, "Starting Shortly in...", 6, 11, 0, oled);
      }

      break;

    //Select songs on Audrey's SD card
    case SELECT_SONGS:
      {
        oled.drawFrame(0, 0, 128, 64);
        //user wouldn't see this state, would be in the background
        // function to randomly select songs: pickSong();, returns song and songTitle
        gameState = PLAY_SONGS;
        songPlayingTimer = millis();
      }
      break;

    //Play the songs  and if somebody presses button to answer, then the song will stop playing
    case PLAY_SONGS:
      {
        oled.drawFrame(0, 0, 128, 64);
        int button_state = digitalRead(BUTTON_PIN);
        oled_print("Points:" + String(playerScore), 5, 10, u8g2_font_profont11_mf);
        oled_print("Round:" + String(Round), 85, 10, u8g2_font_profont11_mf);
        if (millis() - songPlayingTimer <= 10000 && !hint_given) {
          oled_print("Hint shown in", 5, 60, u8g2_font_profont11_mf);
          oled_print(String(10 - (millis() - songPlayingTimer) / 1000), 88, 60, u8g2_font_timB24_tf);
        }
        else if (millis() - songPlayingTimer <= 10000 && hint_given) {
          oled_print("Round end in", 5, 60, u8g2_font_profont11_mf);
          oled_print(String(10 - (millis() - songPlayingTimer) / 1000), 88, 60, u8g2_font_timB24_tf);
        }
        oled.setFont(u8g2_font_profont11_mf);
        if ((millis() - songPlayingTimer) > 10000) {
          hintRevealTimer = millis();
          gameState = HINT_REVEAL;
        }
        if (flag_1 == 1) { //adjust to register which player pressed the button
          gameState = PROCESS_VOICE;
        }
        if (millis() - songPlayingTimer > 10000 && hint_given) {
          playerScore -= roundPoints;
          lockedplayer_post("newlock");
          gameState = LOCK_PLAYER;
        }
      }
      break;

    case HINT_REVEAL:
      {
        oled.drawFrame(0, 0, 128, 64);
        oled_print("The Hint is", 20, 20, u8g2_font_helvB12_tf);
        oled_print("'Artist Name'", 15, 45, u8g2_font_helvB12_tf);
        hintCounter += 1;
        //reveal hint function: hintReveal();
        if (millis() - hintRevealTimer > 4000 || flag_1 == 1) {
          songPlayingTimer = millis();
          hint_given = true;
          gameState = PLAY_SONGS;
        }
      }
      break;

    //Using Hieu's and David's voice program, we will set the name of the songs to be predetermined
    //In this function, if the server side script returns True, then update the score, if it's false then locks the player from the round
    case PROCESS_VOICE:
      {
        oled.drawFrame(0, 0, 128, 64);
        oled.setFont(u8g2_font_unifont_t_symbols);
        oled.drawGlyph(25, 59, 0x21e9);
        oled_print("Hold to Answer", 10, 35, u8g2_font_t0_16b_tf  );
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
        oled.drawFrame(0, 0, 128, 64);
        Serial.println(userGuess);
        //String url = "/608dev/sandbox/ekoto4/ServerHandler.py?state=checkguess&songIndex=6&guess=" + userGuess;
        String isGuessCorrect = DoGet(userGuess);//"GET", "iesc-s1.mit.edu", url);
        userGuess = "";
        Serial.println(isGuessCorrect);
        if (isGuessCorrect == "TRUE") {
          playerScore += roundPoints;
          oled_print("Correct!", 12, 30, u8g2_font_crox3cb_tr);
          oled.sendBuffer();
          leaderboard_post(username_message, playerScore);
          delay(1000);
          gameState = UPDATE_SCORE;
        }
        else if (isGuessCorrect == "FALSE") {
          oled_print("Incorrect", 5, 30, u8g2_font_crox3cb_tr);
          oled_print("You can no longer answer", 10, 45, u8g2_font_haxrcorp4089_t_cyrillic);
          playerScore -= roundPoints;
          lockedplayer_post("newlock");
          oled.sendBuffer();
          delay(1000);
          gameState = LOCK_PLAYER;
        }
      }
      break;

    case LOCK_PLAYER: //STATE INCOMPLETE!!!!!!!!!!!!!!
      {
        oled.drawFrame(0, 0, 128, 64);
        //function that locks player x from current round, lockPlayer()
        lockedplayer_get();
        // lockedPlayers += 1;
        if (numberoflocks == "5") {
          playerScore -= roundPoints;
          //  lockedPlayers=0;
          lockedplayer_post("reset");
          leaderboard_post(username_message, playerScore);
          gameState = UPDATE_SCORE;
        }
        else {                          //NEED TO EDIT
          songPlayingTimer = millis();
          lockedplayer_post("reset");
          gameState = PLAY_SONGS;
          //warning: we need to cover the case in which the last player(s) don't press anything and jsut waste time listening to the song, doing nothing, it's stuck how we currently have the cases
        }
        oled.sendBuffer();
        delay(2000);
      }
      break;

    case UPDATE_SCORE:
      {
        oled.drawFrame(0, 0, 128, 64);
        //function that updates score of player x, updateScore()
        //player wouldn't see this state, would be taking place in the background
        leaderboard_post(username_message, playerScore);
        userGuess = "";
        oled_print("Scoreboard", 25, 10, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 1), 20, 20, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 2), 20, 30, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 3), 20, 40, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 4), 20, 50, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 5), 20, 60, u8g2_font_victoriabold8_8r );
        oled.sendBuffer();
        delay(2000);
        gameState = UPDATE_ROUND;
      }
      break;

    //When updating round, update the score of the player, maybe nice interface and what not
    case UPDATE_ROUND:
      {
        oled.drawFrame(0, 0, 128, 64);
        playersLocked = 0;
        hint_given = false;
        hintCounter = 0;
        Round += 1;
        //roundPoints *= Round;
        oled_print("Round " + String(Round), 20, 35, u8g2_font_crox3cb_tr);
        oled.sendBuffer();
        delay(2000);
        if (Round < 10) {
          songPlayingTimer = millis();
          gameState = SELECT_SONGS;

        }
        else {
          leaderboard_get();
          gameState = FINALIZE_WINNER;
        }
      }
      break;

    //In the end, have a scoreboard of everybody who has played in that current round, each round will reset the scoreboard
    case FINALIZE_WINNER:
      {
        oled.drawFrame(0, 0, 128, 64);
        oled_print("Scoreboard", 25, 10, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 1), 20, 20, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 2), 20, 30, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 3), 20, 40, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 4), 20, 50, u8g2_font_victoriabold8_8r );
        oled_print( parse_username(leaderboard, 5), 20, 60, u8g2_font_victoriabold8_8r );
        //function to display winner points, etc. showWinner()
        if (flag_1 == 1) {
          songindex_newlist();
          gameState = NEW_GAME;
          restart_timer = millis();
          restart = true;
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

void oled_print(String input, int startx, int starty, const uint8_t* font) {
  oled.setFont(font);
  oled.setCursor(startx, starty);
  oled.print(input);
  // update the screen
}
//bool isSongGuessCorrect(int _songIndex, String _guess) {
//
//}

String parse_username(String text, int index_of_commma) {
  int comma_counter = 0;
  for (int i = 0; i < text.length(); i++) {
    if (text.substring(i, i + 1) == ",") {
      comma_counter += 1;
    }
    if (comma_counter == index_of_commma) {
      return text.substring(i + 1, text.indexOf(",", i + 1));
    }
  }
  return "";
}

void songindex_get() {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) {
    client.println("GET http://iesc-s1.mit.edu/608dev/sandbox/daiyunli/songindex.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.print("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back (Header portion)
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line) --> seperates the header from the body
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    songstring = "";
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      songstring += (char)client.read();
    }
    Serial.println(readyplayer_get_output);
    client.stop();
    Serial.println();
    Serial.println("----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
void songindex_newlist() {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "variable=none";
    client.println("POST /608dev/sandbox/daiyunli/songindex.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(thing.length()));
    client.print("\r\n");
    client.print(thing);
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
    songstring = "";
    while (client.available()) { //read out remaining text (body of response)
      songstring += (char)client.read();
    }
    Serial.println(readyplayer_post_output);
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
void lockedplayer_get() {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) {
    client.println("GET http://iesc-s1.mit.edu/608dev/sandbox/daiyunli/lockplayer.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.print("\r\n");
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back (Header portion)
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line) --> seperates the header from the body
        break;
      }
      if (millis() - count > response_timeout) break;
    }
    numberoflocks = "";
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      numberoflocks += (char)client.read();
    }
    Serial.println(readyplayer_get_output);
    client.stop();
    Serial.println();
    Serial.println("----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
void lockedplayer_post(String lockstatus) {
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "newlock=" + lockstatus;
    client.println("POST /608dev/sandbox/daiyunli/lockplayer.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(thing.length()));
    client.print("\r\n");
    client.print(thing);
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
    numberoflocks = "";
    while (client.available()) { //read out remaining text (body of response)
      numberoflocks += (char)client.read();
    }
    Serial.println(readyplayer_post_output);
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
void songindex_listmaker(String songstring) {
  int comma_counter = 0;
  for (int i = 0; i < songstring.length(); i++) {
    if (songstring.substring(i, i + 1) == ",") {
      comma_counter += 1;
      songindex[comma_counter - 1] = (songstring.substring(i + 1, songstring.indexOf(",", i + 1))).toInt();
    }
  }
}
