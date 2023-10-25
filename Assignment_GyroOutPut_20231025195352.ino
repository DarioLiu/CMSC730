#include <Wire.h>
const int sda = 21;
const int scl = 22;

byte GYRO_XOUT_H = 0;
byte GYRO_XOUT_L = 0;
byte GYRO_YOUT_H = 0;
byte GYRO_YOUT_L = 0;
byte GYRO_ZOUT_H = 0;
byte GYRO_ZOUT_L = 0;

int currentTime = 0;
float elapsedTime = 0.0;
int previousTime = 0;

unsigned long caliX = 0;
unsigned long caliY = 0;
unsigned long caliZ = 0;


float gyroAngleX = 0.0;
float gyroAngleY = 0.0;
float gyroAngleZ = 0.0;


int selector = 0;

int address = 0x68;

int16_t GYRO_X_RAW = 0;
int16_t GYRO_Y_RAW = 0;
int16_t GYRO_Z_RAW = 0;

int16_t gyroX_Per_s = 0;
int16_t gyroY_Per_s= 0;
int16_t gyroZ_Per_s = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(sda, scl);

  // Wire.beginTransmission(address); // added code from the class
  // Wire.write(0x1C);
  // Wire.write(0b00011000);
  // Wire.endTransmission();
  // // AcceX = (Wire.read() << 8 | Wire.read() / 2048.0);
  // // AcceY = (Wire.read() << 8 | Wire.read() / 2048.0);
  // // AcceZ = (Wire.read() << 8 | Wire.read() / 2048.0);

  // Wire.beginTransmission(address);
  // Wire.write(0x1B);
  // Wire.write(0x00);
  // Wire.endTransmission();

 

}

void loop() {
  // Requesting high byte of ACCEL_X
  // Wire.beginTransmission(address);
  // Wire.write(0x3B); // Starting with register 0x3B (ACCEL_XOUT_H)
  // Wire.endTransmission(false); // Restart, not stop. This is important for multiple reads.
  // Wire.requestFrom(address, 6, true); // Read 6 registers in a row

  // ACCEL_X_RAW = Wire.read() << 8 | Wire.read();
  // ACCEL_Y_RAW = Wire.read() << 8 | Wire.read();
  // ACCEL_Z_RAW = Wire.read() << 8 | Wire.read();

  // gX = ACCEL_X_RAW / 16384.0;
  // gY = ACCEL_Y_RAW / 16384.0; 
  // gZ = ACCEL_Z_RAW / 16384.0;   // Floating point division
  
  // Serial.print("gX: ");
  // Serial.print(gX);
  // Serial.print(",");
  // Serial.print(gY);
  // Serial.print(",");
  // Serial.println(gZ); 

  // Read Gyro Data
  Wire.beginTransmission(address);
  Wire.write(0x43); // Starting with register 0x43 (GYRO_XOUT_H)
  Wire.endTransmission();
  Wire.requestFrom(address, 6); // Read 6 registers for XYZ gyro

  GYRO_X_RAW = Wire.read() << 8 | Wire.read();
  GYRO_Y_RAW = Wire.read() << 8 | Wire.read();
  GYRO_Z_RAW = Wire.read() << 8 | Wire.read();

  gyroX_Per_s = GYRO_X_RAW/131.0; 
  gyroY_Per_s = GYRO_Y_RAW/131.0;
  gyroZ_Per_s = GYRO_Z_RAW/131.0; 


  previousTime = currentTime;     
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;

  float gyroAngleX_X = gyroAngleX + gyroX_Per_s * elapsedTime;
  float gyroAngleY_Y = gyroAngleY + gyroY_Per_s * elapsedTime;
  float gyroAngleZ_Z = gyroAngleZ + gyroZ_Per_s * elapsedTime;

  gyroAngleX = gyroAngleX_X;
  gyroAngleY = gyroAngleY_Y;
  gyroAngleZ = gyroAngleZ_Z;


  Serial.print(gyroAngleX_X);
  Serial.print(",");
  Serial.print(gyroAngleY_Y);
  Serial.print(",");
  Serial.println(gyroAngleZ_Z);
   
  delay(200);

}


