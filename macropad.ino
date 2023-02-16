
//detta är vår vackra kod, vårt mästerverk, vår skapelse
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include <Wire.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <Keyboard.h>

#define  DEFAULT_I2C_ADDR 0x30
#define  ANALOGIN   18
#define  NEOPIXELOUT 14

Adafruit_seesaw seesaw;
seesaw_NeoPixel pixles = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);
// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
// Create the OLED display
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &SPI1, OLED_DC, OLED_RST, OLED_CS);
// Create the rotary encoder
RotaryEncoder encoder(PIN_ROTA, PIN_ROTB, RotaryEncoder::LatchMode::FOUR3);
void checkPosition() {  encoder.tick(); } // just call tick() to check the state.

// our encoder position state
int encoder_pos = 0;
int enc_rotation = 0;
int profilenum = 0; //state variable current profile
int RGBstate = true;//state variable RGB
int RGBswirl = true;
int Clr = 0;
int brightness = 100;
uint16_t slide_val = 0;
uint16_t lastSlideValue = 0;

void playFusion(int x){
  Keyboard.begin();
  switch(x){
    case 1: Keyboard.write('e'); break;
    case 2: Keyboard.write('d'); break;
    case 3: Keyboard.write('f'); break;
    case 4: Keyboard.write('r'); break;
    case 5: Keyboard.write('c'); break;
    case 6: Keyboard.press(0x81); Keyboard.press('f'); Keyboard.releaseAll();break;
    case 7: 
    case 10:Keyboard.press(0x81); Keyboard.press('x'); Keyboard.releaseAll();break;     //custom, parameters
    case 11: Keyboard.press(0x83); Keyboard.press('z'); Keyboard.releaseAll();break;    //undo
    case 12:Keyboard.press(0x81); Keyboard.press('i'); Keyboard.releaseAll();break;     //custom shortcut, interference
  
  }
  Keyboard.end();
}

void VScode(int x){
  Keyboard.begin();
  switch(x){
    case 1: Keyboard.press(0x83); Keyboard.press(0x81); Keyboard.press('p'); Keyboard.releaseAll(); break; //CMD+SHIFT+P
    case 2: Keyboard.press(0x82); Keyboard.press(0x83); Keyboard.press('u'); Keyboard.releaseAll(); break;//upload
    case 3: Keyboard.press(0x82); Keyboard.press(0x83); Keyboard.press('r'); Keyboard.releaseAll(); break;
    case 4: Keyboard.write(0x64); break;
    case 5: Keyboard.write(0x65); break;
    case 6: Keyboard.write(0x66); break;
    case 7: Keyboard.write(0x67); break;
    case 8: Keyboard.write(0x68); break;
    case 9: Keyboard.write(0x69); break;
    case 10: Keyboard.write(0x70); break;
    case 11: Keyboard.write(0x71); break;
    case 12: Keyboard.write(0x72); break;
  }
  Keyboard.end();
}

void skrivaText(int x){
  Keyboard.begin();
  switch(x){
    case 1: Keyboard.write('a'); break;
    case 2: Keyboard.press(0x81); Keyboard.press(0x66); Keyboard.releaseAll();break;
    case 3: Keyboard.write(0x63); break;
    case 4: Keyboard.write(0x64); break;
    case 5: Keyboard.write(0x65); break;
    case 6: Keyboard.write(0x66); break;
    case 7: Keyboard.write(0x67); break;
    case 8: Keyboard.write(0x68); break;
    case 9: Keyboard.write(0x69); break;
    case 10: Keyboard.write(0x70); break;
    case 11: Keyboard.write(0x71); break;
    case 12: Keyboard.write(0x72); break;
  }
  Keyboard.end();
}

void settings(int x){
  switch(x){
    case 1: RGBstate = !RGBstate; break;
    case 2: RGBswirl = !RGBswirl; break;
    case 3: Clr++ ;Clr = Clr % 12; break;
  }
}

void volume(int x, int y){
  Keyboard.begin();
  if (x - y >= 6.25){//volume increase
    Keyboard.write(0xCD);
    lastSlideValue = slide_val;
  }
  if (x - y <= -6.25){//volume decrease
    Keyboard.write(0xCC);
    lastSlideValue = slide_val;
  }
  Keyboard.end();
}

void docSize(int x, int y){
  Keyboard.begin();
  if (x - y >= 10){//document zoom increase
    Keyboard.press(0x83);
    Keyboard.press('+');
    Keyboard.releaseAll();
  }
  if (x - y <= -10){//document zoom decrease
    Keyboard.press(0x83);
    Keyboard.press('-');
    Keyboard.releaseAll();
  }
  Keyboard.end();
}

void setup() {
  Serial.begin(115200);
  //while (!Serial) { delay(10); }     // wait till serial port is opened
  delay(100);  // RP2040 delay is not a bad idea

  Serial.println("Adafruit Macropad with RP2040");

  // start pixels!
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

  // Start OLED
  display.begin(0, true); // we dont use the i2c address but we will reset!
  display.display();

  
  // set all mechanical keys to inputs
  for (uint8_t i=0; i<=12; i++) {
    pinMode(i, INPUT_PULLUP);
  }



  // set rotary encoder inputs and interrupts
  pinMode(PIN_ROTA, INPUT_PULLUP);
  pinMode(PIN_ROTB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ROTA), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ROTB), checkPosition, CHANGE);  

  // We will use I2C for scanning the Stemma QT port
  Wire.begin();

  if (!seesaw.begin(DEFAULT_I2C_ADDR)) {
    Serial.println(F("seesaw not found!"));
    while(1) delay(10);
  }

  // text display tests
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(SH110X_WHITE, SH110X_BLACK); // white text, black background

}

uint8_t j = 0; // 
bool i2c_found[128] = {false}; //

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  display.println("  GYARB Makroplatta");


  
  encoder.tick();          // check the encoder
  int newPos = encoder.getPosition() * -1;  //flipping to clockwise
  if (encoder_pos != newPos) {//if position has changed
    Serial.print("Encoder:");
    Serial.print(newPos);
    Serial.print(" Direction:");
    Serial.println((int)(encoder.getDirection()));
    encoder_pos = newPos;
    enc_rotation = (20 + (encoder_pos%20)) % 20;

  }
  display.setCursor(0, 8);
  display.print("Profile: ");
  switch(enc_rotation/2){
    case 0: display.print("Settings");break;
    case 1: display.print("Fusion360");break;
    case 2: display.print("VSCode");break;
    case 3: display.print("text editor");break;
    case 4: display.print("Profile 4");break;
    case 5: display.print("Profile 5");break;
    case 6: display.print("Profile 6");break;
    case 7: display.print("Profile 7");break;
    case 8: display.print("Profile 8");break;
    case 9: display.print("Profile 9");break;

  }

  // read the potentiometer
  slide_val = seesaw.analogRead(ANALOGIN);
  display.setCursor(0, 24);
  display.print("Slider: ");
  slide_val = map(slide_val, 0, 1023, 0, 100);
  display.print(slide_val);

  switch(profilenum){
    case 0: brightness = map(slide_val, 0, 100, 0, 255); break;//increase brightness of LEDs
    case 3: docSize(slide_val, lastSlideValue); break;//increase or decrease zoom on document
    default: volume(slide_val, lastSlideValue); break;//increase or decrease volume
  }

  // Scanning takes a while so we don't do it all the time
  if ((j & 0x3F) == 0) {
    Serial.println("Scanning I2C: ");
    Serial.print("Found I2C address 0x");
    for (uint8_t address = 0; address <= 0x7F; address++) {
      Wire.beginTransmission(address);
      i2c_found[address] = (Wire.endTransmission () == 0);
      if (i2c_found[address]) {
        Serial.print("0x");
        Serial.print(address, HEX);
        Serial.print(", ");
      }
    }
    Serial.println();
  }

  // check encoder press
  display.setCursor(0, 16);
  if (!digitalRead(PIN_SWITCH)) {

    Serial.println("Encoder button");
    display.print("Encoder pressed ");
    profilenum = enc_rotation/2; //byter profil
  }

  if(RGBstate == 1){
    pixels.setBrightness(brightness);
    Serial.println("state" + String(RGBstate));
    if(RGBswirl == 1){//rgb cycle active
      Serial.println("swirl" + String(RGBswirl));
      for(int i=0; i< pixels.numPixels(); i++) {//macropad neopixels
        pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
      }
      for(int i; i<pixles.numPixels();i++){//slider neopixels
        pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
      }
      j++;
    
    }
    if(RGBswirl == 0){
      Serial.println("color" + String(Clr));
      for(int i=0; i< pixels.numPixels(); i++) {
        switch(Clr){
          case 0: pixels.setPixelColor(i, 0xFF0000); break; //red
          case 1: pixels.setPixelColor(i, 0xff8000); break; //orange
          case 2: pixels.setPixelColor(i, 0xFFFF00); break; //yellow
          case 3: pixels.setPixelColor(i, 0x80FF00); break; //chartruese
          case 4: pixels.setPixelColor(i, 0x00FF00); break; //green
          case 5: pixels.setPixelColor(i, 0x00FF80); break; //spring green
          case 6: pixels.setPixelColor(i, 0x00FFFF); break; //cyan
          case 7: pixels.setPixelColor(i, 0x0080FF); break; //dodger blue
          case 8: pixels.setPixelColor(i, 0x0000FF); break; //blue
          case 9: pixels.setPixelColor(i, 0x8000FF); break;//purple
          case 10: pixels.setPixelColor(i, 0xFF00FF); break;//violet
          case 11: pixels.setPixelColor(i, 0xFF0080); break;//magenta
        }
      }
    }
  } 
  else{
    pixels.setBrightness(0);
  }
  
  for (int i=1; i<=12; i++) {
    if (!digitalRead(i)) { // switch pressed!
      Serial.println(profilenum);
      switch(profilenum){//depending on profile
        case 0 : settings(i); break;
        case 1 : playFusion(i); break;
        case 2 : VScode(i); break;
        case 3 : skrivaText(i); break;
        //etc 10 ggr
      }

      pixels.setPixelColor(i-1, 0xFFFFFF);  // make white
      // move the text into a 3x4 grid
      display.setCursor(((i-1) % 3)*48, 32 + ((i-1)/3)*8);
      display.print("KEY");
      display.print(i);
    }
    while(!digitalRead(i)){}
  }

  // show neopixels, incredment swirl
  pixels.show();



  // display oled
  display.display();
}





// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}