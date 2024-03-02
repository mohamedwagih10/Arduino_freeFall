#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

const byte rxPin = 8;
const byte txPin = 9;
SoftwareSerial bt(rxPin, txPin);



LiquidCrystal_I2C lcd(0x27, 20, 4);


#define magnet 11
#define sensor 0  

#define led1 2
#define led2 3
#define led3 4

int distance = 0, distance1 = 0, distance2 = 0, distance3 = 0;  // distance in cm

float a1 = 0.0, a2 = 0.0, a3 = 0.0, avg = 0.0 ;  // the acceleration

unsigned long receivedByte = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("      Welcome       ");
  lcd.setCursor(0, 1);
  lcd.print("         To     ");
  lcd.setCursor(0, 2);

  lcd.print("     Free  Fall       ");
  lcd.setCursor(0, 3);

  lcd.print("     Experiment        ");




  Serial.begin(9600);  // Initialize serial communication for debugging
  bt.begin(9600);
  pinMode(magnet, OUTPUT);
 
  pinMode(sensor, INPUT);

  digitalWrite(magnet, LOW);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
}

void loop() {
  if (bt.available()) {
    receivedByte = bt.read();
    if (receivedByte == 150) {
      a1 = a2 = a3 = 0;
      Serial.println("start");
      distance = 1;
      digitalWrite(magnet, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Distance |   Time ");
      lcd.setCursor(0, 1);
      lcd.print("D1=");
      lcd.setCursor(9, 1);
      lcd.print("|T1=");
      

    } else if ((receivedByte == 151) && (distance >= 1) && (distance <= 3)) {
      Serial.print("reset");
      distance--;
      Serial.println(distance);
     
    } else if ((receivedByte > 5 && receivedByte < 110) && distance >= 1 && distance <= 3) {
      switch (distance) {
        case 1:
          distance1 = receivedByte;
          calculateAcceleration(1, distance1, a1);
           avg = (a1);
          break;
        case 2:
          distance2 = receivedByte;
          calculateAcceleration(2, distance2, a2);
          avg = (a1 + a2) / 2;
          break;
        case 3:
          distance3 = receivedByte;
          calculateAcceleration(3, distance3, a3);
          avg = (a1 + a2 + a3) / 3;
          digitalWrite(magnet, LOW);
          break;
      }
      distance++;  // Averge Accerleration of Gravity=9.81 m/s²
    } else if (receivedByte == 1) digitalWrite(magnet, LOW), bt.print(avg), lcd.clear(), lcd.setCursor(0, 0), lcd.print("Average Acceleration  "), lcd.setCursor(5, 1), lcd.print("of Gravity"),lcd.setCursor(0, 2),lcd.print("---> "), lcd.print(avg),  lcd.print("  m/s2 <---") ,lcd.setCursor(0, 3), lcd.print("  Error  = "),lcd.print((abs(avg-9.81)/9.81) * 100), lcd.print(" %");

  }
  updateLEDState(a1, led1);
  updateLEDState(a2, led2);
  updateLEDState(a3, led3);
  if (distance == 1 && distance == 2 && distance == 3) lcd.setCursor(0, 0), lcd.print("Distance   Time "), lcd.setCursor(0, 1), lcd.print("D ="), Serial.println(distance); 
 }

void calculateAcceleration(int dist, int dist_val,  float &acceleration) {
  float time_val = 0.0;
  digitalWrite(magnet, LOW);
  delay(10);
  unsigned long beginTime = millis();
  while (digitalRead(sensor) ) {
    delay(10);
  }
  
  unsigned long endTime = millis();
  time_val = (endTime - beginTime) / 1000.0;  // seconds
  acceleration = 2 * dist_val / (100 * time_val * time_val);
  acceleration = abs(acceleration);
  time_val = time_val * 1000;
  bt.print(time_val);

   /*
  Serial.print("Distance ");
  Serial.print(dist);
  Serial.print(" = ");
  Serial.print(dist_val);
  Serial.print(" , Time ");
  Serial.print(dist);
  Serial.print(" = ");
  Serial.print(time_val);
  Serial.print(", Acceleration ");
  Serial.print(dist);
  Serial.print(" = ");
  Serial.println(acceleration);
  Serial.print("Average Acceleration: ");
  Serial.println(avg);*/

  // to print the distance on lcd
  lcd.setCursor(0, distance);
  lcd.print("D");
  lcd.print(dist);
  lcd.print("= ");
  lcd.setCursor(4, distance);
  lcd.print(dist_val), 
  lcd.print(" cm");

  //to print the time
  lcd.setCursor(9, distance);
  lcd.print("|T");
  lcd.print(dist);
  lcd.print("= ");
  lcd.setCursor(14, distance);
  lcd.print(time_val);
  lcd.setCursor(17, distance);
  lcd.print(" ms");
  digitalWrite(magnet, HIGH);
}
void updateLEDState(int aValue, int ledPin) {
  if (aValue != 0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}