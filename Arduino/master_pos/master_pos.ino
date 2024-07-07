/* LEADER ARDUINO*/

#include <PID_v1.h>
#include <EnableInterrupt.h>
#include <Wire.h>

// Pin Definitions
#define MOTOR1_PWM_PIN 9
#define MOTOR1_IN1_PIN 8
#define MOTOR1_IN2_PIN 12
#define MOTOR2_PWM_PIN 10
#define MOTOR2_IN1_PIN 14
#define MOTOR2_IN2_PIN 15
#define MOTOR3_PWM_PIN 11
#define MOTOR3_IN1_PIN 16
#define MOTOR3_IN2_PIN 17
#define ENCODER1_PIN_A 2
#define ENCODER1_PIN_B 3
#define ENCODER2_PIN_A 4
#define ENCODER2_PIN_B 5
#define ENCODER3_PIN_A 7
#define ENCODER3_PIN_B 6

// Variables
double targetpos1 = 0, targetpos2 = 0, targetpos3 = 0;
double output1, output2, output3;
double pos1 = 0, pos2 = 0, pos3 = 0;
int move = -1;
int i = 0;

int states[3] = {0, 0, 0};

// PID Parameters
double kp = 2, ki = 0.5, kd = 1;
PID motor1PID(&pos1, &output1, &targetpos1, kp, ki, kd, DIRECT);
PID motor2PID(&pos2, &output2, &targetpos2, kp, ki, kd, DIRECT);
PID motor3PID(&pos3, &output3, &targetpos3, kp, ki, kd, DIRECT);

int receivedData = -1; // Variable to store data received from Serial

void computePID(){
  if(abs(pos1 - targetpos1) > 10){
    motor1PID.Compute();
    analogWrite(MOTOR1_PWM_PIN, abs(output1));
  }
  if(abs(pos2 - targetpos2) > 10){
    motor2PID.Compute();
    analogWrite(MOTOR2_PWM_PIN, abs(output2));
  }
  if(abs(pos3 - targetpos3) > 10){
    motor3PID.Compute();
    analogWrite(MOTOR3_PWM_PIN, abs(output3));
  }
}

void setMotor(int output, int MOTOR_IN1_PIN, int MOTOR_IN2_PIN){
  digitalWrite(MOTOR_IN1_PIN, HIGH);
  digitalWrite(MOTOR_IN2_PIN, LOW);
}

void stopMotor(int MOTOR_IN1_PIN, int MOTOR_IN2_PIN){
  digitalWrite(MOTOR_IN1_PIN, LOW);
  digitalWrite(MOTOR_IN2_PIN, LOW);
}

void forward(){
  if(i == 0){
    targetpos1 = 1700, targetpos2 = 1700, targetpos3 = 1700;
    computePID(); // Apply PID Output to motor PWM Pins
    if(states[0] == 0){setMotor(output1, MOTOR1_IN2_PIN, MOTOR1_IN1_PIN);}
    if(states[1] == 0){setMotor(output2, MOTOR2_IN2_PIN, MOTOR2_IN1_PIN);}
    if(states[2] == 0){setMotor(output3, MOTOR3_IN2_PIN, MOTOR3_IN1_PIN);}
    if(abs(pos1 - 1650) < 140){
      stopMotor(MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
      states[0] = 1;
      pos1 -= 1800;
    }
    if(abs(pos2 - 1650) < 140){
      stopMotor(MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
      states[1] = 1;
      pos2 -= 1800;
    }
    if(abs(pos3 - 1650) < 140){
      stopMotor(MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);
      states[2] = 1;
      pos3 -= 1800;
    }
    if(states[0] == 1 && states[1] == 1 && states[2] == 1){
      i = 1;
      delay(100);
      sendStatus(i);
      delay(100);
      for(int k = 0;k < 3;k++){
        states[k] = 0;
      }
    }
  }
}

void backward(){
  if(i == 0){
    targetpos1 = -1700, targetpos2 = -1700, targetpos3 = -1700;
    computePID(); // Apply PID Output to motor PWM Pins
    if(states[0] == 0){setMotor(output1, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);}
    if(states[1] == 0){setMotor(output2, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);}
    if(states[2] == 0){setMotor(output3, MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);}
    if(abs(pos1 + 1650) < 140){
      stopMotor(MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
      states[0] = 1;
      pos1 += 1800;
    }
    if(abs(pos2 + 1650) < 140){
      stopMotor(MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
      states[1] = 1;
      pos2 += 1800;
    }
    if(abs(pos3 + 1650) < 140){
      stopMotor(MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);
      states[2] = 1;
      pos3 += 1800;
    }
    if(states[0] == 1 && states[1] == 1 && states[2] == 1){
      i = 1;
      delay(100);
      sendStatus(i);
      delay(100);
      for(int k = 0;k < 3;k++){
        states[k] = 0;
      }
    }
  }
}

void rightF(){
  if(i == 0){
    targetpos1 = 0, targetpos2 = 1700, targetpos3 = 0;
    computePID(); // Apply PID Output to motor PWM Pins
    if(states[1] == 0){setMotor(output2, MOTOR2_IN2_PIN, MOTOR2_IN1_PIN);}
    if(abs(pos2 - 1650) < 140){
      stopMotor(MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
      states[1] = 1;
      pos2 -= 1800;
    }
    if(states[1] == 1){
      i = 1;
      delay(100);
      sendStatus(i);
      delay(100);
      for(int k = 0;k < 3;k++){
        states[k] = 0;
      }
    }
  }
}

void rightB(){
  if(i == 0){
    targetpos1 = 0, targetpos2 = -1700, targetpos3 = 0;
    computePID(); // Apply PID Output to motor PWM Pins
    if(states[1] == 0){setMotor(output2, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);}
    if(abs(pos2 + 1650) < 140){
      stopMotor(MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
      states[1] = 1;
      pos2 += 1800;
    }
    if(states[1] == 1){
      i = 1;
      delay(100);
      sendStatus(i);
      delay(100);
      for(int k = 0;k < 3;k++){
        states[k] = 0;
      }
    }
  }
}

void leftF(){
  if(i == 0){
    targetpos1 = 1700, targetpos2 = 0, targetpos3 = 1700;
    computePID(); // Apply PID Output to motor PWM Pins
    if(states[0] == 0){setMotor(output1, MOTOR1_IN2_PIN, MOTOR1_IN1_PIN);}
    if(states[2] == 0){setMotor(output3, MOTOR3_IN2_PIN, MOTOR3_IN1_PIN);}
    if(abs(pos1 - 1650) < 140){
      stopMotor(MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
      states[0] = 1;
      pos1 -= 1800;
    }
    if(abs(pos3 - 1650) < 140){
      stopMotor(MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);
      states[2] = 1;
      pos3 -= 1800;
    }
    if(states[0] == 1 && states[2] == 1){
      i = 1;
      delay(100);
      sendStatus(i);
      delay(100);
      for(int k = 0;k < 3;k++){
        states[k] = 0;
      }
    }
  }
}

void leftB(){
  if(i == 0){
    targetpos1 = -1700, targetpos2 = 0, targetpos3 = -1700;
    computePID(); // Apply PID Output to motor PWM Pins
    if(states[0] == 0){setMotor(output1, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);}
    if(states[2] == 0){setMotor(output3, MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);}
    if(abs(pos1 + 1650) < 140){
      stopMotor(MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
      states[0] = 1;
      pos1 += 1800;
    }
    if(abs(pos3 + 1650) < 140){
      stopMotor(MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);
      states[2] = 1;
      pos3 += 1800;
    }
    if(states[0] == 1 && states[2] == 1){
      i = 1;
      delay(100);
      sendStatus(i);
      delay(100);
      for(int k = 0;k < 3;k++){
        states[k] = 0;
      }
    }
  }
}

void stop(){
  stopMotor(MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
  stopMotor(MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
  stopMotor(MOTOR3_IN1_PIN, MOTOR3_IN2_PIN);
}

void setup() {
  Serial.begin(9600);
  Wire.begin(1);
  Wire.onReceive(receiveEvent);

  pinMode(MOTOR1_PWM_PIN, OUTPUT);
  pinMode(MOTOR1_IN1_PIN, OUTPUT);
  pinMode(MOTOR1_IN2_PIN, OUTPUT);
  pinMode(MOTOR2_PWM_PIN, OUTPUT);
  pinMode(MOTOR2_IN1_PIN, OUTPUT);
  pinMode(MOTOR2_IN2_PIN, OUTPUT);
  pinMode(MOTOR3_PWM_PIN, OUTPUT);
  pinMode(MOTOR3_IN1_PIN, OUTPUT);
  pinMode(MOTOR3_IN2_PIN, OUTPUT);

  pinMode(ENCODER1_PIN_A, INPUT);
  pinMode(ENCODER1_PIN_B, INPUT);
  pinMode(ENCODER2_PIN_A, INPUT);
  pinMode(ENCODER2_PIN_B, INPUT);
  pinMode(ENCODER3_PIN_A, INPUT);
  pinMode(ENCODER3_PIN_B, INPUT);

  // Initialize encoder pins
  enableInterrupt(ENCODER1_PIN_A, updatePos1, CHANGE);
  enableInterrupt(ENCODER1_PIN_B, updatePos1, CHANGE);
  enableInterrupt(ENCODER2_PIN_A, updatePos2, CHANGE);
  enableInterrupt(ENCODER2_PIN_B, updatePos2, CHANGE);
  enableInterrupt(ENCODER3_PIN_A, updatePos3, CHANGE);
  enableInterrupt(ENCODER3_PIN_B, updatePos3, CHANGE);

  // Initialize PID
  motor1PID.SetMode(AUTOMATIC);
  motor2PID.SetMode(AUTOMATIC);
  motor3PID.SetMode(AUTOMATIC);
}

void loop() {
  if (Serial.available() > 0) {
    String x = Serial.readString(); // Read the data
    receivedData = x.toInt();
    if (receivedData >= 0 && receivedData <= 6) {
      Serial.print("MOVE DATA: ");Serial.println(receivedData);
      move = receivedData;
      sendToFollower(receivedData);
    }
  }

  if(move == 0){stop();}
  else if(move == 1){forward();}
  else if(move == 2){backward();}
  else if(move == 3){rightF();}
  else if(move == 4){rightB();}
  else if(move == 5){leftF();}
  else if(move == 6){leftB();}

}

void receiveEvent(int howMany){
  if(Wire.available()){
    i = Wire.read();
  }
}

// Function to send data to the follower
void sendToFollower(int data) {
  Wire.beginTransmission(8); // Transmit to device #8
  Wire.write(1);             // Identifier Move
  Wire.write(data);          // Send the data
  Wire.endTransmission();    // Stop transmitting
}

void sendStatus(int status){
  Wire.beginTransmission(8);
  Wire.write(2); // Identifier Status
  Wire.write(status);
  Wire.endTransmission();
}

// Interrupt service routine to update encoder ticks for motor 1
void updatePos1() {
    static unsigned char enc1_prev=0;
    unsigned char enc1_new = (digitalRead(ENCODER1_PIN_A) << 1) | digitalRead(ENCODER1_PIN_B);
    if (enc1_prev == 0x03 && enc1_new == 0x02) pos1++;
    else if (enc1_prev == 0x02 && enc1_new == 0x03) pos1--;
    else if (enc1_prev == 0x00 && enc1_new == 0x01) pos1++;
    else if (enc1_prev == 0x01 && enc1_new == 0x00) pos1--;
    enc1_prev = enc1_new;
    if(pos1 > 1800){
      pos1 -= 1780;
    }
    else if(pos1 < -1800){
      pos1 += 1780;
    }
}

void updatePos2() {
    static unsigned char enc2_prev=0;
    unsigned char enc2_new = (digitalRead(ENCODER2_PIN_A) << 1) | digitalRead(ENCODER2_PIN_B);
    if (enc2_prev == 0x03 && enc2_new == 0x02) pos2++;
    else if (enc2_prev == 0x02 && enc2_new == 0x03) pos2--;
    else if (enc2_prev == 0x00 && enc2_new == 0x01) pos2++;
    else if (enc2_prev == 0x01 && enc2_new == 0x00) pos2--;
    enc2_prev = enc2_new;
    if(pos2 > 1800){
      pos2 -= 1780;
    }
    else if(pos2 < -1800){
      pos2 += 1780;
    }
}

void updatePos3() {
    static unsigned char enc3_prev=0;
    unsigned char enc3_new = (digitalRead(ENCODER3_PIN_A) << 1) | digitalRead(ENCODER3_PIN_B);
    if (enc3_prev == 0x03 && enc3_new == 0x02) pos3++;
    else if (enc3_prev == 0x02 && enc3_new == 0x03) pos3--;
    else if (enc3_prev == 0x00 && enc3_new == 0x01) pos3++;
    else if (enc3_prev == 0x01 && enc3_new == 0x00) pos3--;
    enc3_prev = enc3_new;
    if(pos3 > 1800){
      pos3 -= 1780;
    }
    else if(pos3 < -1800){
      pos3 += 1780;
    }
}
