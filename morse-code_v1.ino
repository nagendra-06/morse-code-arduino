/* OLED DISPLAY (I2C):
 * SDA -> A4 
 * SCL -> 
 * VCC -> 3.3V
 * GND -> GND
 * 
 * BUZZER:
 * + -> D4
 * - -> GND
 * 
 * BUTTON:
 * One side -> D2
 * Other side -> GND
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN 2  // Button between D2 and GND
#define BUZZER_PIN 4  // Buzzer+ to D4, buzzer- to GND

// Morse timing in milliseconds (more forgiving)
const int DOT_DURATION = 200;      // 0.2s dot
const int DASH_DURATION = 400;     // 0.4s dash
const int SYMBOL_GAP = 200;        // 0.2s between symbols
const int LETTER_TIMEOUT = 1000;   // 1s to complete letter
const int WORD_TIMEOUT = 3000;     // 2s for word space

String morseInput = "";
String decodedMessage = "";
unsigned long pressStart = 0;
unsigned long lastRelease = 0;
bool buttonDown = false;
bool buzzerActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  initializeDisplay();
  testBuzzer();
}

void loop() {
  handleButton();
  checkTimeouts();
}

void handleButton() {
  // Button pressed (LOW because using INPUT_PULLUP)
  if (digitalRead(BUTTON_PIN) == LOW && !buttonDown) {
    buttonDown = true;
    pressStart = millis();
    startBuzzer();
    Serial.println("Button pressed");
  }
  
  // Button released
  if (digitalRead(BUTTON_PIN) == HIGH && buttonDown) {
    buttonDown = false;
    unsigned long pressDuration = millis() - pressStart;
    stopBuzzer();
    
    // Determine dot or dash with visual feedback
    if (pressDuration < 400) { // Under 0.4s = dot
      morseInput += ".";
      blinkDot();
    } else { // 0.4s or longer = dash
      morseInput += "-";
      blinkDash();
    }
    
    lastRelease = millis();
    
    // Check for clear command ".....-"
    if (morseInput == ".....-") {
      clearDisplay();
    } else {
      updateDisplay();
    }
  }
}

void checkTimeouts() {
  unsigned long currentTime = millis();
  
  // Check for letter completion
  if (morseInput.length() > 0 && (currentTime - lastRelease > LETTER_TIMEOUT)) {
    decodeLetter();
  }
  
  // Check for word space
  if (decodedMessage.length() > 0 && 
      (currentTime - lastRelease > WORD_TIMEOUT) && 
      decodedMessage.charAt(decodedMessage.length()-1) != ' ') {
    decodedMessage += ' ';
    updateDisplay();
    Serial.println("Added word space");
  }
}

void decodeLetter() {
  char letter = '?';
  
  // Full Morse alphabet lookup
  if (morseInput == ".-") letter = 'A';
  else if (morseInput == "-...") letter = 'B';
  else if (morseInput == "-.-.") letter = 'C';
  else if (morseInput == "-..") letter = 'D';
  else if (morseInput == ".") letter = 'E';
  else if (morseInput == "..-.") letter = 'F';
  else if (morseInput == "--.") letter = 'G';
  else if (morseInput == "....") letter = 'H';
  else if (morseInput == "..") letter = 'I';
  else if (morseInput == ".---") letter = 'J';
  else if (morseInput == "-.-") letter = 'K';
  else if (morseInput == ".-..") letter = 'L';
  else if (morseInput == "--") letter = 'M';
  else if (morseInput == "-.") letter = 'N';
  else if (morseInput == "---") letter = 'O';
  else if (morseInput == ".--.") letter = 'P';
  else if (morseInput == "--.-") letter = 'Q';
  else if (morseInput == ".-.") letter = 'R';
  else if (morseInput == "...") letter = 'S';
  else if (morseInput == "-") letter = 'T';
  else if (morseInput == "..-") letter = 'U';
  else if (morseInput == "...-") letter = 'V';
  else if (morseInput == ".--") letter = 'W';
  else if (morseInput == "-..-") letter = 'X';
  else if (morseInput == "-.--") letter = 'Y';
  else if (morseInput == "--..") letter = 'Z';
  
  if (letter != '?') {
    decodedMessage += letter;
    Serial.print("Decoded: "); Serial.println(letter);
  }
  
  morseInput = "";
  updateDisplay();
}

void clearDisplay() {
  decodedMessage = "";
  morseInput = "";
  updateDisplay();
  // Visual feedback that clear was executed
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  Serial.println("Display cleared");
}

void startBuzzer() {
  tone(BUZZER_PIN, 800); // Standard Morse frequency
  buzzerActive = true;
}

void stopBuzzer() {
  noTone(BUZZER_PIN);
  buzzerActive = false;
}

void testBuzzer() {
  // Play Morse "V" (...-) as test
  tone(BUZZER_PIN, 800, DOT_DURATION); delay(DOT_DURATION + SYMBOL_GAP);
  tone(BUZZER_PIN, 800, DOT_DURATION); delay(DOT_DURATION + SYMBOL_GAP);
  tone(BUZZER_PIN, 800, DOT_DURATION); delay(DOT_DURATION + SYMBOL_GAP);
  tone(BUZZER_PIN, 800, DASH_DURATION); delay(DASH_DURATION + LETTER_TIMEOUT);
  noTone(BUZZER_PIN);
}

void blinkDot() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}

void blinkDash() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
}

void initializeDisplay() {
  if(!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println(F("Display failed"));
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(F("MORSE DECODER"));
  display.println(F("Short=. (0.2s)"));
  display.println(F("Long=- (0.4s+)"));
  display.println(F("800Hz tone"));
  display.println(F(".....- to clear"));
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Input: ")); display.println(morseInput);
  display.setCursor(0, 20);
  display.println(F("Message:"));
  display.setCursor(0, 30);
  display.println(decodedMessage);
  display.display();
}