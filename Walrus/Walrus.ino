#include <LiquidCrystal.h>

#define LEDBLUE 9
#define LEDRED 8
#define BUTTON 12
#define BACKSPACE 13
#define BUZZER 11
#define NOTE_A3  220

//DECODE
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const int columns = 16;
const int rows = 2;
int lcdindex = 0;
int line1[columns];
int line2[columns];
int lastPrint;                                                  // index of the last printed character
bool prevLine = false;                                          // used to check if a whole row was deleted by pressing the backsapace button 

int unit3 = 350;                                                // 3 units - dash time
char code[20];

int stateLedBlue = LOW;
int stateLedRed = LOW;

bool up = true;
int stateButton;
int lastStateButton = LOW;
int stateBKSP;
int lastStateBKSP = LOW;

unsigned long timeElapsedPressed;
unsigned long timeElapsedNOTPressed = -1;
unsigned long lastDebounceTime = 0; 
unsigned long lastDebounceTimeBKSP = 0; 
unsigned long debounceDelay = 50;

//ENCODE
String text;
String ccode;
int unit = 85;                                                   // dot time  

int selector = -1;

void setup() {
  pinMode(LEDBLUE, OUTPUT);
  pinMode(LEDRED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BACKSPACE, INPUT_PULLUP);

  lcd.begin(columns, rows);
  lcd.clear();
  lcd.setCursor(0, 0);                                           // (col, row)

  Serial.begin(9600);
}

void loop() {
  selectMode();
}

void selectMode() {
  int stateButton = digitalRead(BUTTON);
  int stateBKSP = digitalRead(BACKSPACE);
  
  if (selector == -1) {
    if (stateBKSP == HIGH) {
      selector = 1;
    }
    if (stateButton == HIGH) {
      selector = 2;
      delay(70);
    }
  }
  
  if (selector == 1) {                                          // Encode: text to audio cw
    Encode();
  }
  else if (selector == 2) {                                     // Decode: cw(button) to text(LCD)
    Decode();
  }  
}

void Decode() {
  int reading = digitalRead(BUTTON);
  int deletebtn = digitalRead(BACKSPACE);

// send cw by pressing the button
  if (reading != lastStateButton) {
      lastDebounceTime = millis();                              // debounce - reset the timer
  }

  if ( (millis() - lastDebounceTime) > debounceDelay ) {        
    if (reading != stateButton) {                               
      stateButton = reading;                                    
      
      if (stateButton == HIGH) {                                // pressed
      tone(BUZZER, NOTE_A3);
        if (up == true) {
          timeElapsedPressed = millis();
          up = false;
          if (timeElapsedNOTPressed > 0) {                      // a dot or a line was already pressed
            timeElapsedNOTPressed = millis() - timeElapsedNOTPressed;
//          if (timeElapsedNOTPressed <= unit3) {               // space within a character
//          }
            if (timeElapsedNOTPressed > unit3 && timeElapsedNOTPressed <= 2*unit3) {        // space between character
              decodeCWtoASCII();
              strcpy(code, "");
              Serial.print("/");
            }
            if (timeElapsedNOTPressed > 2*unit3) {              // space between words
              decodeCWtoASCII();
              strcpy(code, "");
              printASCII(32);   //space
              Serial.println();
            }
          }
        }         
      }
      
      if (stateButton == LOW) { 
        noTone(BUZZER);
        if (up == false) {
          timeElapsedPressed = millis() - timeElapsedPressed;
          if (stateLedBlue == HIGH) {
            strcat(code,".");
            Serial.print(".");
          }
          else if (stateLedRed == HIGH) {
            strcat(code,"-");
            Serial.print("-");
          }
          timeElapsedNOTPressed = millis();
        }
        stateLedBlue = LOW;
        stateLedRed = LOW;
        up = true;
      }
    }
    
    else if (reading = HIGH) {                                  // turn on the "training" LEDs
      if (millis() - timeElapsedPressed <= unit3) {             // dot
        stateLedBlue = HIGH;
        stateLedRed = LOW;
      }
      else if (millis() - timeElapsedPressed > unit3) {         // dash
        stateLedBlue = LOW;
        stateLedRed = HIGH;
      }
    }
  }
  lastStateButton = reading; 

// delete last character on LCD
  if (deletebtn != lastStateBKSP) {
      lastDebounceTimeBKSP = millis();                          // debounce - reset the timer
  }

  if ( (millis() - lastDebounceTimeBKSP) > debounceDelay ) {   
      if (deletebtn != stateBKSP)
      {
            stateBKSP = deletebtn;
            if (stateBKSP == HIGH) {
                deleteascii();
            }
      }
  }
  lastStateBKSP = deletebtn; 
  digitalWrite(LEDBLUE, stateLedBlue);
  digitalWrite(LEDRED, stateLedRed);
}

void deleteascii() {
  strcpy(code, "");                                             // clear last "pressed" cw that didn't show up yet on the LCD
  if (lastPrint < 0) {
    if (line1[columns-1] != '\0') {                             // deleted the (whole) second line => change cursor from 2nd line to 1st one
      lastPrint = columns - 1;
      line1[lastPrint] = '\0';
      lcd.setCursor(lastPrint, 0);
      lcd.write(line1[lastPrint]);
      lastPrint--;
      prevLine = true;
    }
    else {
      lastPrint = 0;
    }
  }
  else {
    if (line1[columns-1] != '\0') {                             // completed the first row => go to the 2nd one
      line2[lastPrint] = '\0';
      lcd.setCursor(lastPrint, 1);
      lcd.write(line2[lastPrint]);
    }
    else {
      line1[lastPrint] = '\0';
      lcd.setCursor(lastPrint, 0);
      lcd.write(line1[lastPrint]);
    }
    lastPrint--;
  }
}
    
void printASCII(int asciinumber) {                              // print the ascii code to the LCD one a time   
  if (lcdindex > columns-1) {                                   // exceeds the length of one LCD line
    if (line2[0] == '\0') {                                     // 2nd line is clear 
      lcd.setCursor(0, 1);
    }
    else {                                                      // we were already writing on the second row => copy 2nd to 1st
      lcd.clear();
      for (int i = 0; i < columns; i++) {
          lcd.setCursor(i, 0);
          lcd.write(line2[i]);
          line1[i] = line2[i];
      }
      lcd.setCursor(0, 1);
    }
    lcdindex = 0;
    line2[lcdindex] = asciinumber;
    lcd.write(asciinumber);
    lastPrint = lcdindex;
    lcdindex++;
  }
  
  else {
    if ( (line2[0] == '\0' && prevLine == true) || line2[0] != '\0' ) {
      if (prevLine == true) { 
        line2[0] = " ";
      }
      prevLine = false;
      line2[lcdindex] = asciinumber;
      lcd.setCursor(lcdindex, 1);
      lcd.write(asciinumber);
    }
    else if (line2[0] == '\0') {
      line1[lcdindex] = asciinumber;
      lcd.setCursor(lcdindex, 0);
      lcd.write(asciinumber);
    }
    lastPrint = lcdindex;
    lcdindex++;
  }
}

void decodeCWtoASCII() {                                        // translate continuous wave (morse code) to ASCII
  if (strcmp(code,".-") == 0) printASCII(65);      // A
  if (strcmp(code,"-...") == 0) printASCII(66);    // B
  if (strcmp(code,"-.-.") == 0) printASCII(67);    // C
  if (strcmp(code,"-..") == 0) printASCII(68);     // D
  if (strcmp(code,".") == 0) printASCII(69);       // E
  if (strcmp(code,"..-.") == 0) printASCII(70);    // F
  if (strcmp(code,"--.") == 0) printASCII(71);     // G
  if (strcmp(code,"....") == 0) printASCII(72);    // H
  if (strcmp(code,"..") == 0) printASCII(73);      // I
  if (strcmp(code,".---") == 0) printASCII(74);    // J
  if (strcmp(code,"-.-") == 0) printASCII(75);     // K
  if (strcmp(code,".-..") == 0) printASCII(76);    // L
  if (strcmp(code,"--") == 0) printASCII(77);      // M
  if (strcmp(code,"-.") == 0) printASCII(78);      // N
  if (strcmp(code,"---") == 0) printASCII(79);     // O
  if (strcmp(code,".--.") == 0) printASCII(80);    // P
  if (strcmp(code,"--.-") == 0) printASCII(81);    // Q
  if (strcmp(code,".-.") == 0) printASCII(82);     // R
  if (strcmp(code,"...") == 0) printASCII(83);     // S
  if (strcmp(code,"-") == 0) printASCII(84);       // T
  if (strcmp(code,"..-") == 0) printASCII(85);     // U
  if (strcmp(code,"...-") == 0) printASCII(86);    // V
  if (strcmp(code,".--") == 0) printASCII(87);     // W
  if (strcmp(code,"-..-") == 0) printASCII(88);    // X
  if (strcmp(code,"-.--") == 0) printASCII(89);    // Y
  if (strcmp(code,"--..") == 0) printASCII(90);    // Z
 
  if (strcmp(code,".----") == 0) printASCII(49);   // 1
  if (strcmp(code,"..---") == 0) printASCII(50);   // 2
  if (strcmp(code,"...--") == 0) printASCII(51);   // 3
  if (strcmp(code,"....-") == 0) printASCII(52);   // 4
  if (strcmp(code,".....") == 0) printASCII(53);   // 5
  if (strcmp(code,"-....") == 0) printASCII(54);   // 6
  if (strcmp(code,"--...") == 0) printASCII(55);   // 7
  if (strcmp(code,"---..") == 0) printASCII(56);   // 8
  if (strcmp(code,"----.") == 0) printASCII(57);   // 9
  if (strcmp(code,"-----") == 0) printASCII(48);   // 0
 
  if (strcmp(code,"..--..") == 0) printASCII(63);  // ?
  if (strcmp(code,".-.-.-") == 0) printASCII(46);  // .
  if (strcmp(code,"--..--") == 0) printASCII(44);  // ,
  if (strcmp(code,"-.-.--") == 0) printASCII(33);  // !
  if (strcmp(code,".--.-.") == 0) printASCII(64);  // @
  if (strcmp(code,"---...") == 0) printASCII(58);  // :
  if (strcmp(code,"-....-") == 0) printASCII(45);  // -
  if (strcmp(code,"-..-.") == 0) printASCII(47);   // /
 
  if (strcmp(code,"-.--.") == 0) printASCII(40);   // (
  if (strcmp(code,"-.--.-") == 0) printASCII(41);  // )
  if (strcmp(code,".-...") == 0) printASCII(95);   // _
  if (strcmp(code,"...-.-") == 0) printASCII(62);  // >
  if (strcmp(code,".-.-.") == 0) printASCII(60);   // <
  if (strcmp(code,"...-.") == 0) printASCII(126);  // ~
  
  if (strcmp(code,".-.-") == 0) printASCII(3);     // END OF TEXT
  if (strcmp(code,"---.") == 0) printASCII(4);     // END OF TRANSMISSION
  if (strcmp(code,".--.-") == 0) printASCII(6);    // ACKNOWLEDGE
}


void Encode() {
  digitalWrite(LEDBLUE, HIGH);
  int switchState = digitalRead(BUTTON);
  if (switchState == HIGH) {
    selector = 2;
    delay(70);
  }
  if (Serial.available() > 0) {
    text = Serial.readString();
    Serial.print(text);
    for (int i = 0; i < text.length()-1; i++) {
      Serial.println(text[i]);
      if (text[i] == ' ') {
        delay(7*unit);
      }
      else {
        encodeASCIItoCW(text[i]);
        Serial.println(ccode);
        for (int j = 0; j < ccode.length()-1; j++) {            // -1 because when pressing "enter" the string is appended cand trimite ia si un enter or sth la final
          sound(ccode[j]);
          delay(unit);
        }
        sound(ccode[ccode.length()-1]);
      }
      if (text[i+1] != ' ') {
        delay(3*unit);
      }
    } 
  }  
}

void sound(char symbol) {
  if (symbol == '.') {
    dot();
  }
  else if (symbol == '-') {
    dash();
  }
}

void dot() {
  tone(BUZZER, NOTE_A3);
  delay(unit);
  noTone(BUZZER);
}

void dash() {
  tone(BUZZER, NOTE_A3);
  delay(3*unit);
  noTone(BUZZER);
}

void encodeASCIItoCW(char c) {                                  // translate continuous wave (morse) code to ASCII
  if (c == 'A') ccode = ".-";      // A
  if (c == 'B') ccode = "-...";    // B
  if (c == 'C') ccode = "-.-.";    // C
  if (c == 'D') ccode = "-..";     // D
  if (c == 'E') ccode = ".";       // E
  if (c == 'F') ccode = "..-.";    // F
  if (c == 'G') ccode = "--.";     // G
  if (c == 'H') ccode = "....";    // H
  if (c == 'I') ccode = "..";      // I
  if (c == 'J') ccode = ".---";    // J
  if (c == 'K') ccode = "-.-";     // K
  if (c == 'L') ccode = ".-..";    // L
  if (c == 'M') ccode = "--";      // M
  if (c == 'N') ccode = "-.";      // N
  if (c == 'O') ccode = "---";     // O
  if (c == 'P') ccode = ".--.";    // P
  if (c == 'Q') ccode = "--.-";    // Q
  if (c == 'R') ccode = ".-.";     // R
  if (c == 'S') ccode = "...";     // S
  if (c == 'T') ccode = "-";       // T
  if (c == 'U') ccode = "..-";     // U
  if (c == 'V') ccode = "...-";    // V
  if (c == 'W') ccode = ".--";     // W
  if (c == 'X') ccode = "-..-";    // X
  if (c == 'Y') ccode = "-.--";    // Y
  if (c == 'Z') ccode = "--..";    // Z
}
