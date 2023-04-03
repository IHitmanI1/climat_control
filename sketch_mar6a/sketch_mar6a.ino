//Подключение необходимых библиотек
#include <SoftwareSerial.h>
#include <Wire.h>                       
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

//создание переменных
const int trigPin = 6;
const int echoPin = 8;
const int led = 3;
const int dhtPin = 2;
const int touchPin = 11;
int value = 0; 
long duration;
int distance = 0;
int realDist;
bool comeIn = false;
bool bLight = false;

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial btSerial(1, 0);
DHT dht;


void setup() {
   //инициализации датчиков
   dht.setup(dhtPin);
   lcd.init();          
   lcd.backlight();
   btSerial.begin(9600);
   //переключение режимов пинов
   pinMode(led, OUTPUT);
   pinMode(echoPin, INPUT);
   pinMode(trigPin, OUTPUT);
   pinMode(touchPin, INPUT);
   pinMode(A0, INPUT);
   delay(500); 
   //измерение расстояния
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(1000);
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   distance = (duration/2)/29.1;
   //подсчёт среднего значения расстояния
   realDist = (distance / 10) * 10;  
   delay(500);     
}

void loop() {
  //проверка на доступность bluetooth-модуля
  if (btSerial.available())
    Serial.write(btSerial.read());
  if (Serial.available())
    btSerial.write(Serial.read());

  //постоянный подсчёт расстояния 
   int Digital = digitalRead(touchPin);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(1000);
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   distance = (duration/2)/29.1;

  //получение данных с датчика DHT11
   float humi  = dht.getHumidity();
   float chumi = humi * 2.1;
   float tempC = dht.getTemperature();
   value = analogRead(A0);

   //проверка на вход человека в комнату
   if(comeIn){
     //проверка уровня освещённости
     if(value > 600){
       digitalWrite(led, HIGH);
     }
     else{
       digitalWrite(led, LOW);
     }
   }
   else{
     digitalWrite(led, LOW);
   }
  //проверка на нужность включения света в комнате
  if(Digital == 1){
    bLight = !bLight;   
  }
  //проверка на включенный свет
  if(bLight){
    //является ли человек в комнате
    if(comeIn){
    //проверка уровня освещённости
     if(value > 600){
       digitalWrite(led, HIGH);
     }
     else{
       digitalWrite(led, LOW);
     }
   }
   else{
     digitalWrite(led, LOW);
   }
  }
  else{
    digitalWrite(led, LOW);
  }
  //проверка расстояния
  if(distance < realDist){
    comeIn = !comeIn;
    delay(500);
  }
  //проверка на то, что человек в комнате
  if(comeIn){
    //если человек в комнате, то на LCD дисплее выводятся данные с датчиков
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp:");
    lcd.print(tempC/100*90, 0);
    lcd.setCursor(9,0);
    lcd.print("Hum:");
    if(chumi >= 100){
      lcd.print(100);
    }
    else{
      lcd.print(chumi, 0);
    }
    lcd.setCursor(3,1);
    lcd.print("Light:");
    if(value > 600 && bLight){
      lcd.print("ON");
    }
    else{
      lcd.print("OFF");
    }
    //передача данных через bluetooth-модуль
    btSerial.println("");
    btSerial.print("Temperature: ");
    btSerial.print(tempC);
    btSerial.print(" °C");
    btSerial.println("");
    btSerial.print("Humidity: ");
    if(chumi >= 100){
      chumi = 100;
    }
    btSerial.print(chumi);
    btSerial.print(" %");

  }
  else{
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Waiting...");
  }
  delay(500);
}

