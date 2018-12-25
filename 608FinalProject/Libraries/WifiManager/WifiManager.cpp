#include <WifiManager.h>

WifiManager::WifiManager(){

}

void WifiManager::postNewState(String _gameState){
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
      if (millis() - count > RESPONSE_TIMEOUT) break;
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

void WifiManager::postUsername(String _userName){
  WiFiClient client; //instantiate a client object
  String serverResponse = "";
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String content = "user="+_userName;
    client.println("POST /608dev/sandbox/hieutan/finalproject/final_project_lobby.py HTTP/1.1");
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
      if (millis()-count > RESPONSE_TIMEOUT) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      serverResponse+=(char)client.read();
    }
    Serial.println(serverResponse);
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

String WifiManager::getOtherPlayersUserNames(){
    WiFiClient client; //instantiate a client object
    String usernames = "";
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
      if (millis()-count>RESPONSE_TIMEOUT) break;
      }
      count = millis();
      while (client.available()) { //read out remaining text (body of response)
        usernames+=(char)client.read();
      }
      Serial.println(usernames);
      client.stop();
      Serial.println();
      Serial.println("----------");
    }else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
    }

    return usernames;
}

String WifiManager::getSongIndexes() {
  WiFiClient client; //instantiate a client object
  String songIndexesString = "";
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
      if (millis()-count>RESPONSE_TIMEOUT) break;
      }
      count = millis();
      while (client.available()) { //read out remaining text (body of response)
        songIndexesString+=(char)client.read();
      }
      Serial.println(songIndexesString);
      client.stop();
      Serial.println();
      Serial.println("----------");
    }else {
      Serial.println("connection failed");
      Serial.println("wait 0.5 sec...");
      client.stop();
    }

    return songIndexesString;

}

String WifiManager::getHint(int _hintNumber, int _songIndex) { //String _method, String _host, String _url) {
  WiFiClient client; //instantiate a client object
  String hint = "";
  if (client.connect("iesc-s1.mit.edu", 80)) {
    // This will send the request to the server
    // If connected, fire off HTTP GET:;
    client.println("GET /608dev/sandbox/ekoto4/ServerHandler.py?state=sendHint&songIndex="+ String(_songIndex) +"&hintNum=" + String(_hintNumber) + " HTTP/1.1");
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
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      hint += (char)client.read();
    }
    Serial.println(hint);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }

  return hint;
}

String WifiManager::checkPlayerGuess(int _songIndex, String _userGuess) { //String _method, String _host, String _url) {
  WiFiClient client; //instantiate a client object
  String userGuess = "";
  if (client.connect("iesc-s1.mit.edu", 80)) {
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    client.println("GET /608dev/sandbox/ekoto4/ServerHandler.py?state=checkguess&songIndex="+ String(_songIndex) +"&guess=" + _userGuess + " HTTP/1.1");
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
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      userGuess += (char)client.read();
    }
    Serial.println(userGuess);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }

  return userGuess;
}

String WifiManager::updateLeaderBoard(String _username, int _playerScore) {
  WiFiClient client; //instantiate a client object
  String leaderBoard = "";
  if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
    String contents = "player=" + _username + "&score=" + _playerScore;
    client.println("POST /608dev/sandbox/daiyunli/leaderboard.py HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(contents.length()));
    client.print("\r\n");
    client.print(contents);
    unsigned long count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      String line = client.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line)
        break;
      }
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    count = millis();
    leaderBoard = "";
    while (client.available()) { //read out remaining text (body of response)
      leaderBoard += (char)client.read();
    }
    Serial.println(leaderBoard);
    client.stop();
    Serial.println();
    Serial.println("-----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
    delay(300);
  }
  return leaderBoard;
}

String WifiManager::getLeaderBoard() {
  WiFiClient client; //instantiate a client object
  String leaderBoard = "";
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
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    leaderBoard = "";
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      leaderBoard += (char)client.read();
    }
    Serial.println(leaderBoard);
    client.stop();
    Serial.println();
    Serial.println("----------");
  } else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
  }

  return leaderBoard;
}

String WifiManager::getNumberOfLockedPlayers() {
  WiFiClient client; //instantiate a client object
  String numberOfLockedPlayers = "";
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
      if (millis()-count>RESPONSE_TIMEOUT) break;
      }
      count = millis();
      while (client.available()) { //read out remaining text (body of response)
        numberOfLockedPlayers+=(char)client.read();
      }
      Serial.println(numberOfLockedPlayers);
      client.stop();
      Serial.println();
      Serial.println("----------");
    }else {
    Serial.println("connection failed");
    Serial.println("wait 0.5 sec...");
    client.stop();
    }

    return numberOfLockedPlayers;
}

void WifiManager::adjustLockedPlayers(String _lockedPlayerStatus){
    WiFiClient client; //instantiate a client object
    String numberOfLockPlayers;
    if (client.connect("iesc-s1.mit.edu", 80)) { //try to connect to class server
    // This will send the request to the server
    // If connected, fire off HTTP GET:
      String contents = "newlock="+_lockedPlayerStatus;
      client.println("POST /608dev/sandbox/daiyunli/lockplayer.py HTTP/1.1");
      client.println("Host: iesc-s1.mit.edu");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(contents.length()));
      client.print("\r\n");
      client.print(contents);
      unsigned long count = millis();
      while (client.connected()) { //while we remain connected read out data coming back
        String line = client.readStringUntil('\n');
        Serial.println(line);
        if (line == "\r") { //found a blank line!
        //headers have been received! (indicated by blank line)
          break;
      }
      if (millis()-count>RESPONSE_TIMEOUT) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      numberOfLockPlayers+=(char)client.read();
    }
    Serial.println(numberOfLockPlayers);
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

String WifiManager::getLatestState(){
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
        if (millis() - count > RESPONSE_TIMEOUT) break;
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

void WifiManager::resetSongs(){
    WiFiClient client; //instantiate a client object
    String songs = "";
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
      if (millis() - count > RESPONSE_TIMEOUT) break;
    }
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      songs += (char)client.read();
    }
    Serial.println(songs);
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