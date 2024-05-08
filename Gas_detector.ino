#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
#include<Keypad.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD I2C address

byte rowPins[4] = {5,6,7,8};
byte colPins[4] = {9,10,11,12};

char keys [4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', '+'}
  };

Keypad manuja = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
String number="";
int rst = 4;
int buzzer = 13;




void setup() {
  digitalWrite(rst, HIGH);
  pinMode(rst, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT");
  updateSerial();

  lcd.begin(16,2);
  lcd.backlight();
  
  lcd.setCursor(4,0);
  lcd.print("Welcome");
  delay(1000);

  lcd.clear();
  delay(1000);

  lcd.setCursor(0,0);
  lcd.print("Enter the number");
  delay(3000);
  
}

void loop() {
  int x = 1;
  
  char key = manuja.getKey();

    if(key != NO_KEY){
      Serial.println(key);
      number = number+key;
      Serial.println(number);
      Serial.print("Number length : ");
      Serial.println(number.length());
      lcd.setCursor(2,1);
      lcd.print(number);
      delay(10);

      if(number.length()==12){

         lcd.setCursor(2,1);
         lcd.print(number);
         delay(5000);

         lcd.clear();
         delay(1000);

         lcd.setCursor(1,0);
         lcd.print("If it is wrong");
         delay(100);

         lcd.setCursor(0,1);
         lcd.print("Press C to clear");
         delay(5000);

         lcd.clear();
         delay(1000);
         
         mySerial.println("AT+CMGF=1"); 
         updateSerial();
         mySerial.print("AT+CMGS=\""); // enter your phone number here (prefix country code)
         mySerial.print(number);
         mySerial.println("\"");
         updateSerial();
         mySerial.println("Gas detecting system is activated"); // enter your message here
         updateSerial();
         mySerial.write(26);
         
         lcd.setCursor(2,0);
         lcd.print("Msg sent");
         delay(5000);

         lcd.clear();
         delay(1000);
         
         while(x>0){
            float gas_value = analogRead(A0);
            Serial.print("Gas value : ");
            Serial.println(gas_value);
                    if(gas_value>500){
                       digitalWrite(buzzer,HIGH);
                       
                       mySerial.print("ATD+ ");
                       mySerial.print(number);
                       mySerial.println(";");
                       delay(20000);//waiting for 20 seconds
                       mySerial.println("ATH");
                       updateSerial();
                       lcd.setCursor(1,0);
                       lcd.print("Gas leaking");
                       delay(10000);
                       x = x+1;
                      }

                    else{
                      digitalWrite(buzzer, LOW);
                      delay(1000);
                      
                      }
         }
        }
      else if(key=='C'){
        Serial.println("Device will restart after 5 seconds");

        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("Shutting down");
        delay(5000);
        digitalWrite(rst, LOW);//arduino reset command
        }
     }
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
