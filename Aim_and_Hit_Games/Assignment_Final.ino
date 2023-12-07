#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0X3C//liushuUsersyuan/Desktop/Dario/iSchool/CMSC730/A3/A3_homework_1/pitches.h

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10
#define LOGO_HEIGHT 16
#define LOGO_WEIGHT 16

int BUZZ_PIN = 12;

int x = SCREEN_WIDTH / 2;
int y = SCREEN_HEIGHT / 2;

int UP_PIN = 19; 
int DOWN_PIN = 18;
int LEFT_PIN = 5;
int RIGHT_PIN = 17;
int dataPin = 16; 

const int sda = 21;
const int scl = 22;

int address = 0x68;

int16_t GYRO_X_RAW = 0;
int16_t GYRO_Y_RAW = 0;
int16_t gyroX_Per_s = 0;
int16_t gyroY_Per_s = 0;

int16_t ACCEL_X_RAW = 0;
int16_t ACCEL_Y_RAW = 0;

int currentTime = 0;
float elapsedTime = 0.0;
int previousTime = 0;

float gyroAngleX = 0.0;
float gyroAngleY = 0.0;

float gyroXOffset = 0.0;
float gyroYOffset = 0.0;
float threshold = 5.0;

//Position of the target pixel
int targetX = 0;
int targetY = 0;

int targetX_2 = 0;
int targetY_2 = 0;

int hitSequence = 0;
int totalRounds = 0;

int hitCount = 0;

int menuSelection = 1;


bool gameStarted = false;

int STICK_PRESS_PIN = 4;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long startTime;
const unsigned long gameTime = 10000;
int targetIntensity;

int melody [ ] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurationsStart[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int melodyWin[] = {NOTE_C2, NOTE_C2, NOTE_D2, NOTE_C2, 0, NOTE_C2, NOTE_C2, NOTE_D2, NOTE_C2 };
int noteDurationsWin[] = {4, 8, 8, 4, 4, 4, 4, 4, 8};

void startGame(){
  startTime = millis();
  randomSeed(analogRead(0));

  targetIntensity = random(0, 256);

  Serial.println("New Game Started!");
  Serial.print("Target intensity: ");
  Serial.println(targetIntensity);
}

void playMelody(int melody[], int noteDurations[], int notes) {
  for (int thisNote = 0; thisNote < notes; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZ_PIN, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZ_PIN);
  }

}


void caliborateGyro(){
  long x_sum = 0;
  for(int i = 0; i < 100; i++){
    Wire.beginTransmission(address);
    Wire.write(0x43);
    Wire.endTransmission();
    Wire.requestFrom(address,2);
    GYRO_X_RAW = Wire.read() << 8 | Wire.read();
    x_sum += GYRO_X_RAW;
    delay(10);
  }

  gyroXOffset = x_sum / 1000.0;

  Serial.print('X offset: ');
  Serial.println(gyroXOffset);

}

void setup() {
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  // display.clearDisplay();
  // display.setTextSize(1.5);
  // display.setTextColor(WHITE);
  // display.setCursor(0, 0);
  // display.println("> Level One");
  // display.println("  Level Two");
  // display.display();

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display(); // Show initial buffer
  delay(2000); // Pause for 2 seconds

  // Display the initial menu
  updateMenuDisplay();
  //pinMode(STICK_PRESS_PIN, INPUT_PULLUP); // Use INPUT_PULLUP for joystick press
  pinMode(UP_PIN, INPUT_PULLUP); // Also consider using INPUT_PULLUP for UP and DOWN if needed
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUZZ_PIN, OUTPUT);

  Wire.begin(sda, scl);

  caliborateGyro();

  randomSeed(analogRead(0));
  targetX = random(SCREEN_WIDTH);
  targetY = random(SCREEN_HEIGHT);
  while (targetX == x && targetY == y) {
    targetX = random(SCREEN_WIDTH);
    targetY = random(SCREEN_HEIGHT);
  }
  targetX_2 = random(SCREEN_WIDTH);
  targetY_2 = random(SCREEN_HEIGHT);
  while (targetX_2 == x && targetY_2 == y) {
    targetX_2 = random(SCREEN_WIDTH);
    targetY_2 = random(SCREEN_HEIGHT);
  }
  
  while(!gameStarted) {

    bool shouldUpdateDisplay = false;
    updateMenuDisplay();
    if (digitalRead(DOWN_PIN) == LOW) {
        menuSelection = 1;
        shouldUpdateDisplay = true;
        Serial.println("UP Pressed - Level One Selected");
    } 
    if (digitalRead(UP_PIN) == LOW) {
        menuSelection = 2;
        shouldUpdateDisplay = true;
        Serial.println("DOWN Pressed - Level Two Selected");
    }
    if (digitalRead(LEFT_PIN) == LOW) {
      if (menuSelection == 1) {
        gameStarted = true;
        Serial.println("LEFT Pressed - Game Started on Level One");
        // Initialize game settings for Level One
      }
      if (menuSelection == 2) {
        gameStarted = true;
        Serial.println("LEFT Pressed - Game Started on Level Two");
        // Initialize game settings for Level Two
      }
    }
    if (shouldUpdateDisplay) {
      updateMenuDisplay();
    }
  }


}


void loop() {
  
  unsigned long currentMillis = millis();

  if (gameStarted && menuSelection == 1) {
    // Level One game logic
    Wire.beginTransmission(address);
    Wire.write(0x43);
    Wire.endTransmission();
    Wire.requestFrom(address, 4);
    GYRO_X_RAW = Wire.read() << 8 | Wire.read();
    GYRO_Y_RAW = Wire.read() << 8 | Wire.read();
    gyroX_Per_s = (GYRO_X_RAW - gyroXOffset) / 131.0;
    gyroY_Per_s = (GYRO_Y_RAW - gyroYOffset) / 131.0;
    previousTime = currentTime;
    currentTime = millis();
    elapsedTime = (currentTime - previousTime) / 1000.0;
    gyroAngleX += gyroX_Per_s * elapsedTime;
    gyroAngleY += gyroY_Per_s * elapsedTime;
    x = map(gyroAngleX, -50, 50,SCREEN_WIDTH,0);
    y = map(gyroAngleY, -50, 50,0,SCREEN_HEIGHT);
    if (gyroAngleX > threshold) {
      y++;
    } 
    else if (gyroAngleX < -threshold) {
      y--;
    }
    if (abs(x - targetX) <= 4 && abs(y - targetY) <= 4) {
      hitCount++;
      do {
        targetX = random(SCREEN_WIDTH);
        targetY = random(SCREEN_HEIGHT);
      } while (targetX == x && targetY == y);
    }
  
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(String(hitCount));
    display.drawPixel(x, y, WHITE);
    display.drawPixel(targetX, targetY, WHITE);
    display.display();
   // playMelody(melodyWin, noteDurationsWin, sizeof(melodyWin)/sizeof(int));
    delay(10);
}

  if (gameStarted && menuSelection == 2) {
    // Level TWO game logic
    Wire.beginTransmission(address);
    Wire.write(0x43);
    Wire.endTransmission();
    Wire.requestFrom(address, 4);
    GYRO_X_RAW = Wire.read() << 8 | Wire.read();
    GYRO_Y_RAW = Wire.read() << 8 | Wire.read();
    gyroX_Per_s = (GYRO_X_RAW - gyroXOffset) / 131.0;
    gyroY_Per_s = (GYRO_Y_RAW - gyroYOffset) / 131.0;
    previousTime = currentTime;
    currentTime = millis();
    elapsedTime = (currentTime - previousTime) / 1000.0;
    gyroAngleX += gyroX_Per_s * elapsedTime;
    gyroAngleY += gyroY_Per_s * elapsedTime;
    x = map(gyroAngleX, -50, 50,SCREEN_WIDTH,0);
    y = map(gyroAngleY, -50, 50,0,SCREEN_HEIGHT);
    if (gyroAngleX > threshold) {
      y++;
    } 
    else if (gyroAngleX < -threshold) {
      y--;
    }

    if (abs(x - targetX) <= 4 && abs(y - targetY) <= 4) {
      hitCount++;
      do {
        targetX = random(SCREEN_WIDTH);
        targetY = random(SCREEN_HEIGHT);
      } while (targetX == x && targetY == y);
    }
    if (abs(x - targetX_2) <= 4 && abs(y - targetY_2) <= 4) {
      hitCount++;
      do {
        targetX_2 = random(SCREEN_WIDTH);
        targetY_2 = random(SCREEN_HEIGHT);
      } while (targetX_2 == x && targetY_2 == y);
    }
  
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(String(hitCount));
    display.drawPixel(x, y, WHITE);
    display.drawPixel(targetX, targetY, WHITE);
    display.drawPixel(targetX_2, targetY_2, WHITE);
    display.display();
   // playMelody(melodyWin, noteDurationsWin, sizeof(melodyWin)/sizeof(int));
    delay(10);
}


  if (hitCount >= 10) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Mission");
        display.println("Completed");
        display.display();
        delay(2000);
        gameStarted = false;
        menuSelection = 1;
        updateMenuDisplay();  // Update display to show menu

      }

  }






void updateMenuDisplay(){
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  if (menuSelection == 1) {
      display.println("> Level One");
      display.println("  Level Two");
    } 
  else{
      display.println("  Level One");
      display.println("> Level Two");
    }

    display.display();

}
  // Logic for other menu items or game states




