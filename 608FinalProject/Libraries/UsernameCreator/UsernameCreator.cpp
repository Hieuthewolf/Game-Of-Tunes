#import <UsernameCreator.h>

UsernameCreator::UsernameCreator(U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI *_oled){
	this->state       = 0;
	this->charIndex   = 0;
	this->usernameMessage  =  "";
	this->scrollingTimer   =  millis();
	this->queryString      =  "";
	this->Sever            = new WifiManager();
	this->Helpers          = new HelperFunctions();
	this->oled             =  _oled;
}

String UsernameCreator::getUsername(){
	return usernameMessage;
}

String UsernameCreator::update(float _angle, int _leftButton, int _rightButton){
	switch (this->state) {
      case 0:
       this->Helpers->prettyPrint(26, 20, "Pick a Username!", 5, 11, 0, *this->oled);
       this->Helpers->prettyPrint(77,40,"Hold This",5,11,0, *this->oled);
       this->oled->setFont(u8g2_font_unifont_t_symbols); 
       this->oled->drawGlyph(92, 59, 0x21e9); 
       
       if (_rightButton == 2) {
            this->state = 1;
            this->charIndex = 1;
            this->scrollingTimer = millis();
            this->queryString = "";
            return this->usernameMessage;
       }
       if (_rightButton == 1) {
          return this->usernameMessage;
       }
       return this->usernameMessage; 
       break;
  
      case 1:
        this->oled->setFont(u8g2_font_crox1tb_tf);
        this->Helpers->prettyPrint(4, 16, "Choose your username", 5, 11, 0, *this->oled);
        this->oled->drawFrame(4,22,113,12);
        this->oled->setFont(u8g2_font_5x7_tf);
        this->Helpers->prettyPrint(50,45,"Hold to Confirm",2,11,0, *this->oled);
        this->oled->setFont(u8g2_font_unifont_t_symbols);
        this->oled->drawGlyph(88, 62, 0x21e9); 
        if (_rightButton == 1 && this->state == 1) { 
            this->queryString += this->alphabet[this->charIndex];
            this->charIndex = 0;
            this->scrollingTimer = millis();
            this->usernameMessage = this->queryString;
        } 
        if (_rightButton == 2 && this->state == 1) {
            this->state = 2;
            return "";
        }

        if (_angle >= this->angleThreshold && millis() - this->scrollingTimer >= this->scrollingThreshold) {
        	this->charIndex += 1;
        	if (this->charIndex > this->alphabet.length()) {
                this->charIndex = 0;
            }

            this->scrollingTimer = millis();
        } else if (_angle <= -this->angleThreshold && millis()-this->scrollingTimer >= this->scrollingThreshold) {
        	this->charIndex -= 1;
        	if (this->charIndex < 0) {
                this->charIndex = this->alphabet.length()-1;
            }

            this->scrollingTimer = millis();
        }
        this->oled->setCursor(0,50);
        return this->queryString+this->alphabet[this->charIndex];
        break;

      case 2:
        this->Helpers->oled_print("Is " + this->usernameMessage + " Correct?", 4, 16, u8g2_font_crox1tb_tf, *this->oled);
        
        if (_rightButton == 1) {
        	this->state = 1;
        	this->queryString = "";
        }

        if (_leftButton == 1){
        	this->Sever->postUsername(this->usernameMessage);
        	this->queryString = "";
        	this->state = 3;
        	return "";
        }

        this->oled->setFont(u8g2_font_unifont_t_symbols);
        this->oled->drawGlyph(25, 57, 0x2714); 
        this->oled->drawGlyph(92, 57, 0x2716); 
        return "";
        break;
       
       case 3:
        this->oled->setFont(u8g2_font_px437wyse700b_mf);
        this->Helpers->prettyPrint(10, 25, "Player:", 6, 11, 0, *this->oled);
        this->Helpers->prettyPrint(10, 45, this->usernameMessage, 6, 11, 0, *this->oled);
        return "";

        break;
    }
}