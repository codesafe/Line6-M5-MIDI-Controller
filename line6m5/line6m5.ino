#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     4



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiOut); 
#define MIDICHAN 1

////////////////////////////////////////////////////////////////////////

#define BANK_PORT   2
#define SEL1_PORT   3
#define SEL2_PORT   4
#define SEL3_PORT   5

#define LED1_PORT   8
    
#define LAST_ALLPRESET_ADDR     100
#define LAST_BANK_ADDR          10
#define LAST_PRESET_ADDR        20

#define MAX_BANK           8  // 3 * 8 = 24
#define MAX_CHANNEL        3

int Presets[MAX_BANK][MAX_CHANNEL] = { 
  {1,2,3}, 
  {4,5,6},
  {7,8,9},
  {10,11,12},
  {13,14,15},
  {16,17,18},
  {19,20,21},
  {22,23,24}};

int lastBank = 0;
int lastPresetPos[MAX_BANK] = { 0, 0, 0, 0, 0 };   // 각 bank별 마지막 preset


const uint8_t PROGMEM Line6_Logo128 [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x30,
0x03, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFC, 0x78,
0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE, 0x78,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x30,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0x00,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0x00,
0x04, 0x1F, 0xC0, 0x03, 0xFF, 0xE7, 0x80, 0x1F, 0x9F, 0xFF, 0xF0, 0x3F, 0xF8, 0x00, 0x7F, 0x00,
0x04, 0x1F, 0xC0, 0x01, 0xFF, 0xE7, 0xE0, 0x1F, 0x9F, 0xFF, 0xF8, 0x3F, 0xC0, 0x30, 0xFF, 0x00,
0x04, 0x1F, 0xC0, 0x00, 0x7F, 0x07, 0xF8, 0x1F, 0x9F, 0xC0, 0x00, 0x3F, 0x83, 0xFF, 0xFF, 0x00,
0x04, 0x1F, 0xC0, 0x00, 0x7F, 0x07, 0xFE, 0x1F, 0x9F, 0xC0, 0x00, 0x3F, 0x06, 0x01, 0xFF, 0x00,
0x04, 0x1F, 0xC0, 0x00, 0x7F, 0x07, 0xFF, 0x9F, 0x9F, 0xFF, 0xF0, 0x3F, 0x00, 0x00, 0x3F, 0x00,
0x04, 0x1F, 0xC0, 0x00, 0x7F, 0x07, 0xE7, 0xFF, 0x9F, 0xFF, 0xF0, 0x3F, 0x03, 0xFC, 0x1F, 0x00,
0x04, 0x1F, 0xC0, 0x00, 0x7F, 0x07, 0xE1, 0xFF, 0x9F, 0xC0, 0x00, 0x3F, 0x03, 0xFC, 0x1F, 0x00,
0x04, 0x1F, 0xC0, 0xF0, 0x7F, 0x07, 0xE0, 0x7F, 0x9F, 0xC0, 0x00, 0x3F, 0x80, 0xE0, 0x3F, 0x00,
0x04, 0x1F, 0xFF, 0xF3, 0xFF, 0xE7, 0xE0, 0x1F, 0x9F, 0xFF, 0xF8, 0x3F, 0xC0, 0x00, 0xFF, 0x00,
0x04, 0x1F, 0xFF, 0xF3, 0xFF, 0xE7, 0xE0, 0x07, 0x9F, 0xFF, 0xF8, 0x3F, 0xFF, 0x1F, 0xFF, 0x00,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0x00,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x00,
0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFE, 0x00,
0x03, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFC, 0x00,
0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


////////////////////////////////////////////////////////////////////////

#define KEY_STATE_NONE      0
#define KEY_STATE_PRESS     1
#define KEY_STATE_RELEASE   2
// channel + bank
int btnState[MAX_CHANNEL+1] = { KEY_STATE_NONE,KEY_STATE_NONE,KEY_STATE_NONE,KEY_STATE_NONE };
unsigned long pressTime[MAX_CHANNEL+1] = { 0,0,0,0 };

int infotype = 0;
unsigned long infoTimer = 0;


////////////////////////////////////////////////////////////////////////


void ReadAllPresets()
{
  int count = 0;
  for(int i=0; i<MAX_BANK; i++)
  {
    for(int j=0; j<MAX_CHANNEL; j++)
    {
      int v = EEPROM.read(LAST_ALLPRESET_ADDR+count);
      count++;
      Presets[i][j] = v;
    }
  }
}

void WriteAllPresets()
{
  int count = 0;
  for(int i=0; i<MAX_BANK; i++)
  {
    for(int j=0; j<MAX_CHANNEL; j++)
    {
      int v = Presets[i][j];
      EEPROM.write(LAST_ALLPRESET_ADDR+count, v);
      count++;
    }
  }  
}

/////////////////////////////////////////////////////////////////////

void WriteLastBank()
{
   EEPROM.write(LAST_BANK_ADDR, lastBank);
}

void ReadLastBank()
{
  lastBank = EEPROM.read(LAST_BANK_ADDR);
  if( lastBank > MAX_BANK )
    lastBank = 0;
}

/////////////////////////////////////////////////////////////////////

void WriteLastPresetPos()
{
  for(int i=0; i<MAX_BANK; i++)
  {
    EEPROM.write(LAST_PRESET_ADDR+i, lastPresetPos[i]);
  }
}

void ReadLastPresetPos()
{
  for(int i=0; i<MAX_BANK; i++)
  {
    lastPresetPos[i] = EEPROM.read(LAST_PRESET_ADDR+i);
  if(lastPresetPos[i] > MAX_CHANNEL)
    lastPresetPos[i] = 0;
  }
}


////////////////////////////////////////////////////////////////////////

void drawLine6Logo(void) 
{
  display.clearDisplay();
  display.drawBitmap( 0, 1, Line6_Logo128, 128, 30, 1);
  display.display();
  delay(3000);
}


void fxOn() 
{
  midiOut.sendControlChange(11, 88, MIDICHAN); 
}

void fxOff() 
{
  midiOut.sendControlChange(11, 10, MIDICHAN); 
}

void drawText(char *str) 
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.print(str); 
  display.display();
}

// Display update
void UpdateDisplay()
{
  /*
  char buffer[20];
  int pos = lastPresetPos[lastBank];
  int m5Preset = Presets[lastBank][pos];
  sprintf(buffer , "%d|%d->%02d", lastBank, pos, m5Preset);
  drawText(buffer);
  */
    infotype = 0;  
    infoTimer = millis();
    UpdateDisplay1();
}

void UpdateDisplay1()
{
  char buffer[20];
  int pos = lastPresetPos[lastBank];
  int m5Preset = Presets[lastBank][pos];
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);  
  display.setCursor(0,0);
  sprintf(buffer , "B:%d C:%d", lastBank, pos);
  display.print(buffer); 

  char buffer2[20];
  display.setTextSize(2);
  display.setCursor(0,18);
  sprintf(buffer2 , "Preset:%02d", m5Preset);
  display.print(buffer2); 
  
  display.display();  
}

void UpdateDisplay2()
{
  char buffer[20];
  int pos = lastPresetPos[lastBank];
  int m5Preset = Presets[lastBank][pos];
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0,10);
  sprintf(buffer, "%d", lastBank);
  display.print(buffer); 
    
  display.setCursor(12,10);
  display.print("|");  
  display.setCursor(24,10);
  sprintf(buffer, "%d", pos);
  display.print(buffer);   
  //sprintf(buffer, "%d | %d", lastBank, pos);


  char buffer2[20];
  display.setTextSize(4);
  display.setCursor(64,0);
  sprintf(buffer2 , "%02d", m5Preset);
  display.print(buffer2); 
  
  display.display();  
}


void SetM5Preset(int m5Preset)
{
  midiOut.sendProgramChange(m5Preset-1, MIDICHAN);
  fxOn();
  //UpdateDisplay2();
  UpdateDisplay();
  WriteLastPresetPos();
}

// Change bank
void PressBank()
{
    if( lastBank + 1 >= MAX_BANK )
      lastBank = 0;
    else
      lastBank++;

    int pos = lastPresetPos[lastBank];
    int m5Preset = Presets[lastBank][pos];

    SetM5Preset(m5Preset);
 }
 
 void PressChannel(int i)
{
  if(lastPresetPos[lastBank] == i || i > MAX_CHANNEL) return;
  lastPresetPos[lastBank] = i;

  //int pos = lastPresetPos[lastBank];
  int m5Preset = Presets[lastBank][i];

  SetM5Preset(m5Preset);
}

void drawLogoTitle()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("LINE6 M5  ");
  display.setTextSize(1);
  display.setCursor(0,18);  
  display.print("Midi Controller v1.0");
  display.display();
  delay(3000);
}

void setup() 
{
  // Must set to Line6 Midi !
  Serial.begin(31250);
  //Serial.begin(115200);
    
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { // Address 0x3C for 128x32
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  drawLine6Logo();

  drawLogoTitle();
  ReadLastBank();
  ReadLastPresetPos();
 
  UpdateDisplay();
  
  //midiOut.sendProgramChange(7,1);
  //SetM5Preset(6);
  //fxOn();
  
  pinMode(BANK_PORT, INPUT_PULLUP);
  pinMode(SEL1_PORT, INPUT_PULLUP);
  pinMode(SEL2_PORT, INPUT_PULLUP);
//pinMode(SEL3_PORT, INPUT);

  pinMode(LED1_PORT, OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(BANK_PORT), switchFn, FALLING);
  
}

unsigned long countTime = 0;
void switchFn()
{
  //state=!state; 
  countTime=millis();
  //timeVal=countTime;
  Serial.print(F("Interrupt\n"));
}

unsigned long debounceDelay = 200; // ms


bool checkPress(int btn)
{
    int state = digitalRead(btn);
    int p = btn - BANK_PORT;
    unsigned long ct = millis();
    
    if( state == LOW )
    {
      if( btnState[p] == KEY_STATE_NONE && pressTime[p] + debounceDelay < ct )
      {
        btnState[p] = KEY_STATE_PRESS;
        Serial.print(F("pushed\n"));
        pressTime[p] = ct;
        
        return true;
      }
    }
    else if( state == HIGH )
    {
      if( btnState[p] != KEY_STATE_NONE )
      {
        btnState[p] = KEY_STATE_NONE;
        pressTime[p] = ct;        
      }
    }
    return false;
}

void OnOffLed()
{
    int pos = lastPresetPos[lastBank];
    digitalWrite(LED1_PORT, LED1_PORT+pos-LED1_PORT == 0 ? HIGH: LOW);
}

void loop()
{
    if(checkPress(BANK_PORT) == true)
      PressBank();    
    if(checkPress(SEL1_PORT) == true)
      PressChannel(0);
    if(checkPress(SEL2_PORT) == true)
      PressChannel(1);
//    if(checkPress(SEL3_PORT) == true)
//      PressChannel(2);      

    OnOffLed();

    if(infotype == 0)
    {
      unsigned long ct = millis();
      if( infoTimer + 4000 < ct )
      {
        UpdateDisplay2();
        infotype = 1;
      }
    }
}
