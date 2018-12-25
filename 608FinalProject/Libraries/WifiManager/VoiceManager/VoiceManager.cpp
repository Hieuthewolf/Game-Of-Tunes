#include <VoiceManager.h>

#define DELAY 1000
#define SAMPLE_FREQ 8000                           // Hz, telephone sample rate
#define SAMPLE_DURATION 5                         // duration of fixed sampling
#define NUM_SAMPLES SAMPLE_FREQ*SAMPLE_DURATION    // number of of samples
#define ENC_LEN (NUM_SAMPLES + 2 - ((NUM_SAMPLES + 2) % 3)) / 3 * 4  // Encoded length of cli

/* CONSTANTS */
//Prefix to POST request:
const String PREFIX = "{\"config\":{\"encoding\":\"MULAW\",\"sampleRateHertz\":8000,\"languageCode\": \"en-US\",\"speechContexts\": [{\"phrases\": [\"Role\", \"Models\", \"Alright\", \"Crank\", \"that\", \"Kiss\", \"me\", \"through\", \"the\", \"phone\", \"plain\", \"jane\", \"finesse\", \"blessings\", \"sunday\", \"candy\", \"3005\", \"knuck\", \"if\", \"you\", \"buck\", \"god’s\", \"plan\", \"controlla\", \"september\", \"swag\", \"surfin\", \"look\", \"alive\", \"thriller\", \"stir\", \"fry\", \"walk\", \"it\", \"talk\",\"all\", \"stars\", \"XO\", \"Tour\", \"life\", \"Redbone\", \"broken\", \"clocks\", \"caroline\", \"child’s\", \"play\", \"trap\", \"queen\", \"gold\", \"digger\", \"ms.\", \"jackson\", \"love\", \"on\", \"top\", \"I\", \"don’t\", \"fuck\", \"with\", \"dreams\", \"marry\", \"lazy\", \"song\", \"treasure\", \"bodak\", \"yellow\", \"wild\", \"thoughts\", \"back\", \"to\", \"I’m\", \"a\", \"nasty\", \"hoe\", \"talk\", \"dirty\", \"trumpets\", \"beautiful\", \"soul\", \"no\", \"air\", \"iSpy\", \"gucci\", \"gang\", \"let\", \"and\", \"nightmares\", \"bad\", \"boujee\", \"classic\", \"so\", \"sick\", \"brain\", \"bottoms\", \"up\", \"slow\", \"motion\", \"or\", \"nah\", \"wavy\", \"watcha\", \"say\", \"run\", \"dry\", \"one\", \"dance\", \"LUV\"]}]},\"audio\": {\"content\":\"";
const String SUFFIX = "\"}}"; //suffix to POST request
const int AUDIO_IN = A0; //pin where microphone is connected
const String API_KEY = "AIzaSyC2nT5F69sBBaldwhMkcf_nLxzpexAMslg";
const unsigned response_timeout = 6000;

/* Global variables*/
const char* ssid     = "MIT GUEST";     // your network SSID (name of wifi network)
const char* password = ""; // your network password
const char*  server = "speech.google.com";  // Server URL

WiFiClientSecure client; //global WiFiClient Secure object

//Below is the ROOT Certificate for Google Speech API authentication (we're doing https so we need this)
//don't change this!!
const char* root_ca = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                      "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                      "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                      "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                      "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                      "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                      "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                      "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                      "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                      "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                      "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                      "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                      "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                      "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                      "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                      "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                      "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                      "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                      "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                      "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                      "-----END CERTIFICATE-----\n"
;

VoiceManager::VoiceManager(const int _BUTTON_PIN, U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled) {
  this->speech_data = "";//global used for collecting speech data
  this->button_state = 0; //used for containing button state and detecting edges
  this->old_button_state = 0; //used for detecting button edges
  this->time_since_sample = 0;
  this->BUTTON_PIN = _BUTTON_PIN;
  this->oled = _oled;
}

String VoiceManager::recordUserGuess() {
  this->button_state = digitalRead(this->BUTTON_PIN);
  String userGuess = "";
  if (!this->button_state && this->button_state != this->old_button_state) {
    client.setCACert(root_ca);
    delay(200);
    Serial.println("listening...");
    this->oled->clearBuffer();    //clear the screen contents
    this->oled->drawStr(0, 15, "listening...");
    this->oled->sendBuffer();     // update the screen
    this->record_audio();
    Serial.println("sending...");
    this->oled->clearBuffer();    //clear the screen contents
    this->oled->drawStr(0, 15, "sending...");
    this->oled->sendBuffer();     // update the screen
    Serial.print("\nStarting connection to server...");
    delay(300);
    bool conn = false;
    for (int i = 0; i < 10; i++) {
      if (client.connect(server, 443)); {
        conn = true;
        break;
      }
      Serial.print(".");
      delay(300);
    }
    if (!conn) {
      Serial.println("Connection failed!");
    } else {
      Serial.println("Connected to server!");
      // Make a HTTP request:
      delay(200);
      client.println("POST https://speech.googleapis.com/v1/speech:recognize?key=" + API_KEY + "  HTTP/1.1");
      client.println("Host: speech.googleapis.com");
      client.println("Content-Type: application/json");
      client.println("Cache-Control: no-cache");
      client.println("Content-Length: " + String(this->speech_data.length()));
      client.print("\r\n");
      int len = this->speech_data.length();
      int ind = 0;
      int jump_size = 3000;
      while (ind < len) {
        delay(100);//experiment with this number!
        if (ind + jump_size < len) client.print(this->speech_data.substring(ind, ind + jump_size));
        else client.print(this->speech_data.substring(ind));
        ind += jump_size;
      }
      //client.print("\r\n\r\n");
      unsigned long count = millis();
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        Serial.print(line);
        if (line == "\r") { //got header of response
          Serial.println("headers received");
          break;
        }
        if (millis() - count > 4000) break;
      }
      Serial.println("Response...");
      count = millis();
      while (!client.available()) {
        delay(100);
        Serial.print(".");
        if (millis() - count > 4000) break;
      }
      Serial.println();
      Serial.println("-----------");
      String op;
      while (client.available()) {
        op += (char)client.read();
      }
      Serial.println(op);
      int trans_id = op.indexOf("transcript");
      if (trans_id != -1) {
        int foll_coll = op.indexOf(":", trans_id);
        int starto = foll_coll + 2; //starting index
        int endo = op.indexOf("\"", starto + 1); //ending index
        userGuess = op.substring(starto + 1, endo);
        this->oled->clearBuffer();    //clear the screen contents
        this->oled->setCursor(0, 15);
        this->oled->print(op.substring(starto + 1, endo));
        this->oled->sendBuffer();     // update the screen
        delay(1000);
      }
      Serial.println("-----------");
      client.stop();
      Serial.println("done");
    }
  }

  this->old_button_state = this->button_state;
  return userGuess;
}


void VoiceManager::record_audio() {
  int sample_num = 0;    // counter for samples
  int enc_index = PREFIX.length() - 1;  // index counter for encoded samples
  float time_between_samples = 1000000 / SAMPLE_FREQ;
  int value = 0;
  uint8_t raw_samples[3];   // 8-bit raw sample data array
  String enc_samples;     // encoded sample data array
  time_since_sample = micros();
  Serial.println(NUM_SAMPLES);
  this->speech_data = PREFIX;
  while (sample_num < NUM_SAMPLES && !button_state) { //read in NUM_SAMPLES worth of audio data
    value = analogRead(AUDIO_IN);  //make measurement
    raw_samples[sample_num % 3] = this->mulaw_encode_fast(value - 1241); //remove 1.0V offset (from 12 bit reading)
    sample_num++;
    if (sample_num % 3 == 0) {
      this->speech_data += base64::encode(raw_samples, 3);
    }

    // wait till next time to read
    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
    button_state = digitalRead(BUTTON_PIN);
  }
  this->speech_data += SUFFIX;
  Serial.println(this->speech_data);
}

int8_t VoiceManager::mulaw_encode_fast(int16_t sample) {
  const uint16_t MULAW_MAX = 0x1FFF;
  const uint16_t MULAW_BIAS = 33;
  uint16_t mask = 0x1000;
  uint8_t sign = 0;
  uint8_t position = 12;
  uint8_t lsb = 0;
  if (sample < 0)
  {
    sample = -sample;
    sign = 0x80;
  }
  sample += MULAW_BIAS;
  if (sample > MULAW_MAX)
  {
    sample = MULAW_MAX;
  }
  for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
    ;
  lsb = (sample >> (position - 4)) & 0x0f;
  return (~(sign | ((position - 5) << 4) | lsb));
}