#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
Servo servo;
Servo servo1;
Servo servo2;

int led_pin=13;          // пин подключения
int button_pin = 10;     // пин кнопки
int buttonState = 0;
int val = 0; 
int inPin = A0;          //датчик Холла

LiquidCrystal_I2C lcd(0x27,16,2);;
char Data[20]; // 20 is the number of chars it can hold
const byte Usercount = 3;

char Master[2][7] = {
  "123456",
  };

char * User[Usercount] = {
  "Master","INVALID"};

byte currentCommand = 0;
byte MasterCount = 6;
byte good_Count = 0;

int user = 0;
bool Password_is_good, User_is_good;
char customKey;

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {
  8,7,6,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  4,3,2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 


void setup() {
  // put your setup code here, to run once:
pinMode(led_pin, OUTPUT); // Инициализируем цифровой вход/выход в режиме выхода.
  pinMode(button_pin, INPUT); // Инициализируем цифровой вход/выход в режиме входа.
  pinMode(inPin, INPUT);
    
  servo.attach(11);
  servo1.attach(12);
  servo.write(90); //Поворачиваем сервопривод в положение 0
  servo1.write(90); //Поворачиваем сервопривод в положение 0
}

void loop()
{
if(digitalRead(inPin)==LOW)//если магнит прислоняем к датчику ... 
 { 
 digitalWrite(13,HIGH);//включаем светодиод 
 servo.write(0);
 } 
 else//если  магнит не прислонен ... 
 { 
 digitalWrite(13,LOW);//выключаем светодиод
 servo.write(170);
 }

if(digitalRead(button_pin)==LOW)//если кнопка нажата ... 
 { 
 digitalWrite(13,HIGH);//включаем светодиод 
 servo1.write(0);
 } 
 else//если не нажата... 
 { 
 digitalWrite(13,LOW);//выключаем 
 servo1.write(170);
 } 

 
}
