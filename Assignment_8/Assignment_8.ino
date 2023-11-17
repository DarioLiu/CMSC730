#include<Wire.h>
int latchPin = 18; // Latch pin of 74HC595 
int clockPin = 5; // Clock pin of 74HC595 
int dataPin = 19; // Data pin of 74HC595 
const int sda = 21;
const int scl = 22;

byte GYRO_XOUT_H = 0;
byte GYRO_XOUT_L = 0;


int currentTime = 0;
float elapsedTime = 0.0;
int previousTime = 0;

float gyroAngleX = 0.0;
float gyroAngleY = 0.0;

int selector = 0;

int address = 0x68;

int16_t GYRO_X_RAW = 0;

int16_t gyroX_Per_s = 0;


float gyroXOffset = 0.0;
float angle = 0.0;



byte led_data = 0;


void setup() 
{
  Serial.begin(9600);
  Wire.begin(sda, scl);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  pinMode(clockPin, OUTPUT);

  calibrateGyro();
  
}


byte mapAngleToLEDs(int angle){
  angle = constrain(angle, -90, 90);
  int led = map(abs(angle), 0, 90, 0, 8);

  byte ledState = 0;

  for(int i = 0; i<led; i++){
    bitSet(ledState, i);
  }

  return ledState;
}

void loop() 
{
  Wire.beginTransmission(address);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(address,2);

  GYRO_X_RAW = Wire.read() << 8 | Wire.read();

  gyroX_Per_s = GYRO_X_RAW /131.0; 

  previousTime = currentTime; // Calculate the elapsed time since the last loop    
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;

  gyroAngleX += gyroX_Per_s * elapsedTime;
  
  Serial.println(gyroAngleX);
  updateShiftRegister();
  led_data = mapAngleToLEDs(gyroAngleX);
  delay(100);
}


void updateShiftRegister()
{
 digitalWrite(latchPin, LOW);
 shiftOut(dataPin, clockPin, LSBFIRST, led_data);
 digitalWrite(latchPin, HIGH);
}


void calibrateGyro() {
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

  gyroXOffset = x_sum /1000.0;

  Serial.print("X offset: ");
  Serial.println(gyroXOffset);

}




// Variable to hold the pattern of which LEDs are currently turned on or off
// Set the bit that controls that LED in the variable ‘led_data'
//Shifts out a byte of data one bit at a time
//putting the latch Pin HIG