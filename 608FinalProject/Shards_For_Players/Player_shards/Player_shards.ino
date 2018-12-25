#include <U8g2lib.h>
#include <WiFi.h>
#define SCREEN U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI

const int response_timeout = 6000; //ms to wait for response from host
const int button_pin_1 = 15;
const int button_pin_2 = 2;
unsigned long timer;

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 148

SCREEN oled(U8G2_R0, 5, 17,16);
int old_reading; //for remember button value and detecting edges

int state;
int input;
String op;
String op_2;
String user_1;
String user_2;
String user_3;

#define intro_screen 0
#define username 1
#define transition 2
#define lobby 3
#define game_screen 4

void setup(){
    Serial.begin(115200); //begin serial
    delay(100); //wait a bit (100 ms)
    WiFi.begin("iPhone (3)", "team32-1"); //attempt to connect to wifi
    oled.begin();     // initialize the OLED
    oled.clearBuffer();    //clear the screen contents
    oled.setFont(u8g2_font_5x7_tf);  //set font on oled  
    oled.setCursor(0,15); //set cursor
    oled.print("Starting"); //print starting message
    oled.sendBuffer(); //send it (so it displays)
    int count = 0; //count used for Wifi check times
    while (WiFi.status() != WL_CONNECTED && count<6) {
      delay(500);
      Serial.print(".");
      count++;
    }
    delay(2000);
    if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
      Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
      oled_print("All Connected");
      delay(500);
    } else { //if we failed to connect just ry again.
      Serial.println(WiFi.status());
      ESP.restart(); // restart the ESP
    }
    timer= millis();
    pinMode(button_pin_1,INPUT_PULLUP);
    pinMode(button_pin_2,INPUT_PULLUP);
    state = 0;
    input = 0;
}


void loop(){
  int b_value_1 = digitalRead(button_pin_1);
  int b_value_2 = digitalRead(button_pin_2);
  if (b_value_1) {
    input = 0;
  }
  else {
    input = 1;
  }
  if (b_value_2) {
    input = 0;
  }
  else {
    input = 1;
  }
  
  switch (state) {
    case intro_screen:
        oled.clearBuffer();
        oled.setFont(u8g2_font_5x7_tf);  //set font on oled 
        pretty_print(0, 12, "Welcome to Game of Tunes!", 6, 11, 0, oled); 
        pretty_print(0, 24, "Press Left to Continue", 6, 11, 0, oled); 
        oled.sendBuffer();
        if (!b_value_1){
          state = username;
          oled.clearBuffer();
          do_POST();
          oled.sendBuffer();
        }
      
    break;
    
    case username: 
        if (!b_value_2) {
          timer = millis();
          state = transition;
        }
        
    break;

    case transition:
    oled.setFont(u8g2_font_5x7_tf);
    oled_print("Initializing Lobby....");
      if (millis() - timer > 2250) {
        state = lobby;
        timer = millis();
      }
    break;
    
    case lobby:
      printinfo();
      oled.setFont(u8g2_font_5x7_tf);
      oled.clearBuffer();
      pretty_print(45, 10, "Players" , 6, 11, 0, oled);
      oled.drawHLine(35, 13, 52);
      user_1 = parse_leaderboard(op_2, 1);
      user_2 = parse_leaderboard(op_2, 2);
      user_3 = parse_leaderboard(op_2, 3);
      
      pretty_print(40, 24, user_1.substring(3,user_1.length()-1), 6, 11, 0, oled);
      pretty_print(40, 35, user_2.substring(2,user_2.length()-1), 6, 11, 0, oled);
      pretty_print(40, 46, user_3.substring(2,user_3.length()-2), 6, 11, 0, oled);

      oled.drawFrame(8, 51, 60, 11);
      oled.drawFrame(80, 51, 40, 11);
      pretty_print(14, 59, "Start Game", 6, 11, 0, oled);
      pretty_print(86, 59, "Cancel", 6, 11, 0, oled);

      oled.sendBuffer();

      if (!b_value_2) {
        state = intro_screen;
      }
      if (!b_value_1) {
        state = game_screen;
      }
    break;

    case game_screen:
    oled_print("Dank Memes");

    if (!b_value_2) {
      state = intro_screen;
    }

    break;
  }
}

String parse_leaderboard(String text,int index_of_commma) {
  int comma_counter = 0;
  for (int i = 0; i < text.length(); i++) {
    if (text.substring(i,i+1) == ",") {
      comma_counter += 1;
    }
    if (comma_counter == index_of_commma) {
      return text.substring(i+1, text.indexOf(",", i+1));
    }
  }
  return "";
}

//helper function to encapsulate basic printing actions
void oled_print(String input){
  oled.clearBuffer();
  oled.setCursor(0,15);
  oled.print(input);
  oled.sendBuffer();     // update the screen
}



void do_POST(){
  WiFiClient client; //instantiate a client object
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String thing = "user=Elorm&shard_x=4&shard_y=21";
    client.println("POST /608dev/sandbox/hieutan/finalproject/final_project_shards.py HTTP/1.1");
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
      if (millis()-count>response_timeout) break;
    }
    count = millis();
    op = "";
    while (client.available()) { //read out remaining text (body of response)
      op+=(char)client.read();
    }
    oled.setFont(u8g2_font_px437wyse700b_mf);
    pretty_print(10, 25, op.substring(0,7), 6, 11, 0, oled);
    pretty_print(10, 45, op.substring(7,op.length()), 6, 11, 0, oled);
    Serial.println(op);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  }else{
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
    delay(300);
  }
}

void printinfo() {
  WiFiClient client; //instantiate a client object
    if (client.connect("iesc-s1.mit.edu", 80)) { 
      client.println("GET http://iesc-s1.mit.edu/608dev/sandbox/hieutan/finalproject/final_project_shards.py HTTP/1.1"); 
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
      if (millis()-count>response_timeout) break;
      }
      count = millis();
      op_2 = ""; //create empty String object
      while (client.available()) { //read out remaining text (body of response)
        op_2+=(char)client.read();
      }
      Serial.println(op_2);
      client.stop();
      Serial.println();
      Serial.println("----------");
    }else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
    }
}

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





