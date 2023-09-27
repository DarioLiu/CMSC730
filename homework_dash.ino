const int ledPin = 22;
const int dotDuration = 100; // dot that is equal to 100ms
const int dashDuration = dotDuration * 3;
const int spaceSameLetter = dotDuration;
const int spaceTwoLetter = dotDuration * 3 ;

const char* numbers [] = {
    "- - - - -", 
    ". - - .", 
    ". . - . .", 
    ". . . - .", 
    ". . . . -", 
    "- - -", // 5
    ". . . . . .", 
    "- - . . ", 
    "- . . . .", 
    "- . . -"
};
void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  

}

void blinkMorse(const char* morse) { // "- - -"
  for(int i = 0;morse[i] != '\0' ; i++ ) {
    switch(morse[i]){
      case '.':
        digitalWrite(ledPin, HIGH);
        delay(dotDuration);
        digitalWrite(ledPin, LOW);
        break;

      case '-':
        digitalWrite(ledPin, HIGH);
        delay(dashDuration);
        digitalWrite(ledPin, LOW);
        break;
      case ' ':
        //digitalWrite(ledPin, HIGH);
        delay(spaceSameLetter);
        //digitalWrite(ledPin, LOW);
        break;
    }
    // if(morse[i+1] != '\0'){
    //   delay(spaceSameLetter);
    
  
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    String input = Serial.readString();
    input.trim();

    for (int i = 0; i < input.length(); i++){
      int number = input[i]-'0';
      if (number >= 0 && number <=9) {
        blinkMorse(numbers[number]);
        if (i < input.length() - 1){
          delay(spaceTwoLetter);
        }


      }
    Serial.print(number);
    }
  }
  

}
