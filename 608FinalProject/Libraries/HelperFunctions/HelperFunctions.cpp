#import <HelperFunctions.h>

HelperFunctions::HelperFunctions(){

}

void HelperFunctions::prettyPrint(int startx, int starty, String input, int fwidth, int fheight, int spacing, SCREEN &display) {
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

void HelperFunctions::oled_print(String input, int startx, int starty, const uint8_t* font, SCREEN &oled) {
  oled.setFont(font);
  oled.setCursor(startx, starty);
  oled.print(input);
  // update the screen
}

String HelperFunctions::parse_username(String text, int index_of_commma) {
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