#include "pitches.h"

const int ledPin = 23;
const int PhotoPin = 36;
const int buzzPin = 16;

int targetIntensity;
unsigned long startTime;
const unsigned long gameTime = 10000;

int melody [ ] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurationsStart[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int melodyWin[] = {NOTE_C2, NOTE_C2, NOTE_D2, NOTE_C2, 0, NOTE_C2, NOTE_C2, NOTE_D2, NOTE_C2 };
int noteDurationsWin[] = {4, 8, 8, 4, 4, 4, 4, 4, 8};

int melodyLose[] = { NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_D3, NOTE_C3};
int noteDurationsLose[] = {  4, 4, 4, 4, 4, 4, 4, 4
};

void startGame() {
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
    tone(buzzPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzPin);
  }

}

void restartGame(){
  Serial.println("Press any key to restart the game");
  while(1) {
    if (Serial.available() > 0 ){
      Serial.read();
      startGame();
      break;
  }
}
}

void setup(){
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(PhotoPin, INPUT);
  startGame();
  digitalWrite(ledPin, HIGH);
  playMelody(melody, noteDurationsStart, 8);
}



void loop() {
  if (millis() - startTime <= gameTime) {
    int lightIntensity = analogRead(PhotoPin);
    int ledBrightness = map(lightIntensity, 0, 4095, 0, 255);
    analogWrite(ledPin, ledBrightness);
    int mappedIntensity = map(lightIntensity, 0, 4095, 0, 255);
    Serial.println(mappedIntensity);

    int diff = abs(mappedIntensity - targetIntensity);
   

    int pitch;

    if(diff == 0) {
      digitalWrite(ledPin, LOW);
      playMelody(melodyWin, noteDurationsWin, sizeof(melodyWin)/sizeof(int));
      while(1);

    } else if (mappedIntensity > targetIntensity){
      pitch = map(diff, 0, 255, 400, 1000);
    } else {
      pitch = map(diff, 0, 255, 1000, 400);
    }
      tone(buzzPin, pitch, 100);
      delay(100);
      noTone(buzzPin);
    } else {
  
      digitalWrite(ledPin, LOW); 
        
      playMelody(melodyLose, noteDurationsLose, sizeof(melodyLose)/sizeof(int));
      restartGame();
    }
  }

  // put your main code here, to run repeatedly:

  
  // for(int i = 0; i < 255; i++){
  //   analogWrite(ledPin, i);
  //   delay(10);
  // }

  
  // for(int i = 255; i >=0; i--){
  //   analogWrite(ledPin, i);
  //   delay(10);
  // }

  // int lightIntensity = analogRead(PhotoPin);

  // int mappedIntensity = map(lightIntensity, 0, 4095, 0, 255);

  // Serial.println(mappedIntensity);

  // analogWrite(ledPin, mappedIntensity);
  // if (lightIntensity >255) {
  //   analogWrite(ledPin, mappedIntensity;


  // }
  // else {
  //   analogWrite(ledPin, LOW);
  // }

  //tone(buzzPin, mappedIntensity);  
  //notone(buzzPin);

  



