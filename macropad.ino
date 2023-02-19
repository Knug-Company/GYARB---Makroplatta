
//detta är vår vackra kod, vårt mästerverk, vår skapelse
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>
#include <Wire.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include <Keyboard.h>


#define  DEFAULT_I2C_ADDR 0x30//i2c adress of slider
#define  ANALOGIN   18  //pin of slider
#define  NEOPIXELOUT 14//pin of pixels on slider

Adafruit_seesaw seesaw;
seesaw_NeoPixel seesawPixels = seesaw_NeoPixel(4, NEOPIXELOUT, NEO_GRB + NEO_KHZ800);
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
int RGBswirl = true;//
int Clr = 0;
int brightness = 100;
uint16_t slide_val = 0;
uint16_t lastSlideValue = 0;

uint8_t j = 0; 
bool i2c_found[128] = {false}; 

void settings(int x){
  switch(x){
    case 1: RGBstate = !RGBstate; break;//RGB on off
    case 2: RGBswirl = !RGBswirl; break;//RGB swirl 
    case 3: if(RGBstate && !RGBswirl){Clr++ ;Clr = Clr % 12;} break;//next static color
  }
}

void playFusion(int x){
  Keyboard.begin();//Starting keyboard functionality
  switch(x){
    case 1: Keyboard.write('e'); break;//extrude
    case 2: Keyboard.write('d'); break;//dimension
    case 3: Keyboard.write('f'); break;//fillet 3d
    case 4: Keyboard.write('r'); break;//rectangle
    case 5: Keyboard.write('c'); break;//circle
    case 6: Keyboard.press(0x81); delay(10); Keyboard.press('f'); delay(10);break;//fillet 2d, custom
    case 7:Keyboard.write('j'); break;//joint
    case 8:Keyboard.press(0x81); delay(10); Keyboard.press('p'); delay(10);break;//offset plane
    case 9: Keyboard.press(0x81); delay(10); Keyboard.press('a'); delay(10);break; //section analysis
    case 10:Keyboard.press(0x81); delay(10); Keyboard.press('x'); delay(10);break;//custom, parameters
    case 11: Keyboard.press(0x83); delay(10); Keyboard.press('z'); delay(10);break;//undo
    case 12:Keyboard.press(0x81); delay(10); Keyboard.press('i'); delay(10);break;//custom shortcut, interference
  
  }
  Keyboard.releaseAll();//releasing all pressed keys
  Keyboard.end();//stopping keyboard functionality
}

void VScode(int x){
  Keyboard.begin();
  switch(x){
    case 1: Keyboard.press(0x83); delay(10); Keyboard.press(0x81); delay(10); Keyboard.press('p'); delay(10); break;//search
    case 2: Keyboard.press(0x82); delay(10); Keyboard.press(0x83); delay(10); Keyboard.press('u'); delay(10); break;//upload
    case 3: Keyboard.press(0x82); delay(10); Keyboard.press(0x83); delay(10); Keyboard.press('r'); delay(10); break;//verify
    case 4: Keyboard.press(0x83); delay(10); Keyboard.press(0x81); delay(10); Keyboard.press('p'); delay(10); Keyboard.releaseAll(); Keyboard.write('sketch'); Keyboard.press(KEY_KP_ENTER); delay(10); break;
    case 5: Keyboard.press(0x83); delay(10); Keyboard.press(0x81); delay(10); Keyboard.press('p'); delay(10); Keyboard.releaseAll(); Keyboard.write('port'); Keyboard.press(0xE0); delay(10); break;
    case 6: Keyboard.press(0x83); delay(10); Keyboard.press(0x81); delay(10); Keyboard.press('p'); delay(10); Keyboard.releaseAll(); Keyboard.write('rd con'); Keyboard.press(0xE0); delay(10); break;
    case 7: Keyboard.press(0x83); delay(10); Keyboard.press(0x82); delay(10); Keyboard.press(0xDA); delay(10); break;//cursor above
    case 8: Keyboard.press(0x83); delay(10); Keyboard.press(0x82); delay(10); Keyboard.press(0xD9); delay(10); break;//cursor below
    case 9: Keyboard.press(0x83); delay(10); Keyboard.press('u'); delay(10); break;//cursor undo
    case 10: Keyboard.press(0x83); delay(10); Keyboard.write('k'); delay(10); Keyboard.write('2'); delay(10); break;//fold lvl2
    case 11: Keyboard.press(0x83); delay(10); Keyboard.write('k'); delay(10); Keyboard.write('l'); delay(10); break;//fold all
    case 12: Keyboard.press(0x83); delay(10); Keyboard.write('k'); delay(10); Keyboard.write('j'); delay(10); break;//unfold
  }
  Keyboard.releaseAll();
  Keyboard.end();
}

void skrivaText(int x){//för google docs
  Keyboard.begin();
  switch(x){
    case 1: Keyboard.press(0x83); delay(10); Keyboard.press('c'); delay(10); break;//copy
    case 2: Keyboard.press(0x83); delay(10); Keyboard.press('v'); delay(10); break;//paste
    case 3: Keyboard.press(0x83); delay(10); Keyboard.press('a'); delay(10); break;//mark all
    case 4: Keyboard.press(0x83); delay(10); Keyboard.press('.'); delay(10); break;//superscript
    case 5: Keyboard.press(0x83); delay(10); Keyboard.press(','); delay(10); break;//subscript
    case 6: Keyboard.press(0x83); delay(10); Keyboard.press(0x81); delay(10);Keyboard.press('x'); delay(10); break;//strikethrough
    case 7: Keyboard.press(0x83); delay(10); Keyboard.press(0x82); delay(10);Keyboard.press('1'); delay(10); break;//heading 1
    case 8: Keyboard.press(0x83); delay(10); Keyboard.press(0x82); delay(10);Keyboard.press('2'); delay(10); break;//heading 2
    case 9: Keyboard.press(0x83); delay(10); Keyboard.press(0x82); delay(10);Keyboard.press('3'); delay(10); break;//heading 3
    case 10:Keyboard.press(0x83); delay(10); Keyboard.press('k'); delay(10);break;//link
    case 11:Keyboard.press(0x83); delay(10); Keyboard.press(0x82); delay(10); Keyboard.press('m'); delay(10); break;//comment
    case 12:Keyboard.press(0x80); delay(10); Keyboard.press(0x82); delay(10);Keyboard.press('i'); delay(10); Keyboard.write('e'); break;//equation
  }
  Keyboard.releaseAll();
  Keyboard.end();
}

void docSize(int x, int y){
  Keyboard.begin();
  if (x - y >= 10){//document zoom increase
    Keyboard.press(0x83);
    delay(10);
    Keyboard.press(0x82);
    delay(10);
    Keyboard.press('+');
    delay(10);
  }
  if (x - y <= -10){//document zoom decrease
    Keyboard.press(0x83);
    delay(10);
    Keyboard.press(0x82);
    delay(10);
    Keyboard.press('-');
    delay(10);
  }
  Keyboard.releaseAll();
  Keyboard.end();
}

void setup() {
  Serial.begin(115200);
  //while (!Serial) { delay(10); }     // wait till serial port is opened
  delay(100);  // RP2040 delay is not a bad idea

  Serial.println("Adafruit Macropad with RP2040");



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

  // start pixels!
  pixels.begin();//macropad pixels
  seesawPixels.begin(DEFAULT_I2C_ADDR);//slider pixels
  pixels.show(); // Initialize all pixels to 'off'
  seesawPixels.show();
}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  display.println("  GYARB Makroplatta");


  
  encoder.tick();          // check the encoder
  int newPos = encoder.getPosition() * -1;  //flipping to clockwise rotation
  if (encoder_pos != newPos) {//if position has changed
    Serial.print("Encoder:");
    Serial.print(newPos);
    Serial.print(" Direction:");
    Serial.println((int)(encoder.getDirection()));
    encoder_pos = newPos;
    enc_rotation = (4 + (encoder_pos%4)) % 4;//variable to cycle between the 4 profiles

  }
  display.setCursor(0, 8);
  display.print("Profile: ");
  switch(enc_rotation){//displaying the profile of current encoder position
    case 0: display.print("Settings");break;
    case 1: display.print("Fusion360");break;
    case 2: display.print("VSCode");break;
    case 3: display.print("Docs");break;
  }
  if(enc_rotation == profilenum){//marking the active profile
    display.print(" *");
  }

  // read the potentiometer
  slide_val = seesaw.analogRead(ANALOGIN);
  display.setCursor(0, 24);
  display.print("Slider: ");
  slide_val = map(slide_val, 0, 1023, 0, 100);
  display.print(slide_val);

  switch(profilenum){
    case 3: docSize(slide_val, lastSlideValue); break;//increase or decrease zoom on document
    default: brightness = map(slide_val, 0, 100, 1, 255); break;
  }


  display.setCursor(0, 16);
  if (!digitalRead(PIN_SWITCH)) {// check encoder press
    display.print("Rrofile switched ");//printing to the display
    profilenum = enc_rotation; //byter profil
  }

  if(RGBstate == 1){
    pixels.setBrightness(brightness);
    seesawPixels.setBrightness(brightness);
    if(RGBswirl == 1){//rgb cycle active
      for(int i=0; i< pixels.numPixels(); i++) {//macropad neopixels
        pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
      }
      for(int i; i<seesawPixels.numPixels();i++){//slider neopixels
        seesawPixels.setPixelColor(i, Wheel(((i * 256 / seesawPixels.numPixels()) + j) & 255));
      }
      j++;
    
    }
    if(RGBswirl == 0){
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
      for(int i=0; i< 4; i++) {
        switch(Clr){
          case 0: seesawPixels.setPixelColor(i, 0xFF0000); break; //red
          case 1: seesawPixels.setPixelColor(i, 0xff8000); break; //orange
          case 2: seesawPixels.setPixelColor(i, 0xFFFF00); break; //yellow
          case 3: seesawPixels.setPixelColor(i, 0x80FF00); break; //chartruese
          case 4: seesawPixels.setPixelColor(i, 0x00FF00); break; //green
          case 5: seesawPixels.setPixelColor(i, 0x00FF80); break; //spring green
          case 6: seesawPixels.setPixelColor(i, 0x00FFFF); break; //cyan
          case 7: seesawPixels.setPixelColor(i, 0x0080FF); break; //dodger blue
          case 8: seesawPixels.setPixelColor(i, 0x0000FF); break; //blue
          case 9: seesawPixels.setPixelColor(i, 0x8000FF); break;//purple
          case 10: seesawPixels.setPixelColor(i, 0xFF00FF); break;//violet
          case 11: seesawPixels.setPixelColor(i, 0xFF0080); break;//magenta
        }
      } 
    }
  }
  else{//no rgb if it is set to off
    pixels.setBrightness(0);
    seesawPixels.setBrightness(0);
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
      display.setCursor(((i-1) % 3)*48, 32 + ((i-1)/3)*8);// move the text into a 3x4 grid
      display.print("KEY");//printing which key just got pressed
      display.print(i);
    }
    while(!digitalRead(i)){}//stops the program while button is pressed to avoid registering more than once
  }

  // let there be light !
  pixels.show();
  seesawPixels.show();



  // display oled
  display.display();
  delay(50);
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