#if defined(LARGE_FLASH)
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu;

// MPU control/status vars
boolean dmpReady = false;  // set true if DMP init was successful
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

void initMPU() {
  Wire.begin();
  mpu.initialize();
  if (mpu.testConnection()) {
#ifdef Serial_DEBUG
    Serial.println("MPU6050 connection successful");
#endif
  }
#ifdef Serial_DEBUG
  else
    Serial.println("MPU6050 connection failed");
#endif

  // load and configure the DMP
#ifdef Serial_DEBUG
  Serial.println("Initializing DMP...");
#endif
  devStatus = mpu.dmpInitialize();

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
#ifdef Serial_DEBUG
    Serial.println("Enabling DMP...");
#endif
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else {
#ifdef Serial_DEBUG
    Serial.print("DMP Initialization failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
#endif
  }
}

void getMPU() {
  if (!dmpReady) return;

  fifoCount = mpu.getFIFOCount();
  if (fifoCount == 1024) {
    mpu.resetFIFO();
#ifdef Serial_DEBUG
    Serial.println("FIFO overflow!");
#endif
  }
  else {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  }
}
#endif
