#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

//---------------------------------------------------------------Настройки датчика стука----------------------------------------------------------------------

const int knockSensor = A7;         // Датчик стука пьезасор.
const int programSwitch = A6;       // Кнопка программирования датчика стукансор

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int threshold = 9;           // Minimum signal from the piezo to register as a knock
const int rejectValue = 25;        // If an individual knock is off by this percentage of a knock we don't unlock..
const int averageRejectValue = 15; // If the average timing of the knocks is off by this percent we don't unlock.
const int knockFadeTime = 150;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)
const int lockTurnTime = 650;      // milliseconds that we run the motor to get it to go a half turn.

const int maximumKnocks = 20;       // Maximum number of knocks to listen for.
const int knockComplete = 1200;     // Longest time to wait for a knock before we assume that it's finished.


// Variables.
int secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."
int knockReadings[maximumKnocks];   // When someone knocks this array fills with delays between knocks.
int knockSensorValue = 0;           // Last reading of the knock sensor.
int programButtonPressed = false;   // Flag so we remember the programming button setting at the end of the cycle.

//-------------------------------------------Конец настройки датчика стука----------------------------------------------------------------------

//объявляем переменную servo типа Servo
Servo servo1;            //Сервопривод сектор 1
Servo servo2;            //Сервопривод сектор 2
Servo servo3;            //Сервопривод сектор 3

int led_pin1=A0;         // Светодиод сектор 1
int led_pin2=A1;         // Светодиод сектор 2
int led_pin3=A2;         // Светодиод сектор 3
int button_pin = 10;     // пин кнопки, флешки, ручки-замыкатора
int buttonState = 0;

#define SensorPin 1     //Датчик препятствия
int isd = 9;             //Звуковой модуль

LiquidCrystal_I2C lcd(0x27,16,2);;
char Data[20]; // 20 is the number of chars it can hold
const byte Usercount = 3;

char Master[2][7] = {  "000000",  };

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

void setup()
{ 
  pinMode(programSwitch, INPUT);
    pinMode(SensorPin, INPUT);
  pinMode(isd, OUTPUT);
  pinMode(led_pin1, OUTPUT); // Инициализируем цифровой вход/выход в режиме выхода.
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin3, OUTPUT);
  digitalWrite(led_pin2,HIGH);
  pinMode(button_pin, INPUT); // Инициализируем цифровой вход/выход в режиме входа.
 
  servo1.attach(11);   // Сервопривод сектор 1 пин 11
  servo2.attach(12);   // Сервопривод сектор 2 пин 12
  servo3.attach(13);   // Сервопривод сектор 3 пин 13
  servo1.write(0);     // Сервопривод сектор 1 положение 0
  servo2.write(0);     // Сервопривод сектор 2 положение 0 
  servo3.write(0);     // Сервопривод сектор 3 положение 0
      
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

  //Serial.begin(9600);                           // Uncomment the Serial.bla lines for debugging.
  //Serial.println("Program start.");             // but feel free to comment them out after it's working right.
  loadsecretCode();
}
 
void loop()
{ 

//-----------------------------------------------------------------------Датчик стука---------------------------------------------------------------------
  knockSensorValue = analogRead(knockSensor);
  
  if (digitalRead(programSwitch)==HIGH){  // is the program button pressed?
    programButtonPressed = true;          // Yes, so lets save that state
    digitalWrite(led_pin3, HIGH);           // and turn on the red light too so we know we're programming.
  } else {
    programButtonPressed = false;
    digitalWrite(led_pin3, LOW);
  }
  
  if (knockSensorValue >=threshold){
    listenToSecretKnock();
  }
  
//-----------------------------------------------------------Модуль пароля и пользователя------------------------------------------------------
  lcd.setCursor(0,0);
  lcd.print("Enter Password");
  customKey = customKeypad.getKey();
  if (customKey)
  {
    Data[currentCommand] = customKey;
    lcd.setCursor(currentCommand,1);
    lcd.print(Data[currentCommand]);
    currentCommand++;
  }

  if(currentCommand == MasterCount)
  {
    delay(1000);
    while(user != 2)
    {
      for(int count = 0; count < MasterCount; count++)
      {
        if(Data[count] == Master[user][count])
        {
          good_Count++;
        }  
      }
      if(good_Count == MasterCount)
      {
        good_Count = 0;
        Password_is_good = true; 
        break;
      }
      else
      { 
        good_Count = 0; 
        user++; 
        Password_is_good = false;
      }
    }

    lcd.setCursor(0,0);
    if(Password_is_good)
    {
      lcd.clear();
      lcd.print("Password is good");
      servo2.write(90);
      delay(5000);
      lcd.setCursor(0,1);
      //lcd.print(User[user]);
      user = 0;
      lcd.clear();
      clearData(); 
    }
    else 
    {
      lcd.clear();
      lcd.print("Password is bad");
      lcd.setCursor(0,1);
      servo2.write(0);
      //lcd.print(User[user]);
      delay(2000);
      user = 0;
      lcd.clear();
      clearData();
    }
  }
//-----------------------------------------------------------Модуль пароля и пользователя------------------------------------------------------


//----------------------------------------------------------------Для смены пароля пользователя-------------------------------------------------------
/*  if(customKey == '*')
  {
    lcd.clear();
    clearData();
    while(User_is_good != true)
    {
      lcd.setCursor(0,0);
      lcd.print("Change Password");
      lcd.setCursor(0,1);
      lcd.print("Enter User: ");
      user = customKeypad.getKey();
      if(user)
      {
        user -= '0'; // convert from char to int
        lcd.print(user);
        if(user < Usercount) 
        { // if within number of users
          delay(1000);
          User_is_good = true; //gets out of while loop
          user -= 1; // ENTER: 1 = master, 2 = user, but the array is 0 for master and 1 for user. This makes it so.
          lcd.clear();
        }
        else 
        {
          delay(1000);
          lcd.clear();
          lcd.print("Invalid User");
          delay(2000);
          lcd.clear(); 
        }
      } 
    }
    User_is_good = false;// stays in while loop
    while(customKey != '#')
    {
      lcd.setCursor(0,0);
      lcd.print(User[user]); // prints out the user
      lcd.setCursor(0,1);
      lcd.print("Press # to confirm");
      customKey = customKeypad.getKey();
      if (customKey)
      {
        Master[user][currentCommand] = customKey;
        lcd.setCursor(currentCommand,2);
        lcd.print(Master[user][currentCommand]);
        MasterCount = currentCommand;
        currentCommand++;
      }
    }
    if(customKey == '#') // press # to store new passwrd for user
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(User[user]);
      lcd.print(" is reset");
      delay(1000);
      clearData();
      lcd.clear();
    }
  }*/
//----------------------------------------------------------------Для смены пароля пользователя-------------------------------------------------------







//----------------------------------------------------------------Модуль датчика препятствия----------------------------------------------------------
int SensorVal = digitalRead(SensorPin);
  //Если обнаружили движение включаем последовательно реле
  if(SensorVal == LOW)
  {
    digitalWrite(isd, HIGH); //Включаем реле 1
    delay(4000); //Задаем пауз между включением первого реле и второго
    digitalWrite(isd, LOW); //Выключаем реле 1
    delay(500); //Задаем пауз между выключением первого реле и второго
   }
//----------------------------------------------------------------Модуль датчика препятствия----------------------------------------------------------






//-------------------------------------------------------------Модуль флешка, ручка-замыкатор-----------------------------------------------
if(digitalRead(button_pin)==LOW)//если кнопка нажата ...
 { 
 digitalWrite(led_pin1,LOW);//включаем светодиод 
 servo1.write(0);
 } 
 else//если не нажата... 
 { 
 digitalWrite(led_pin1,HIGH);//выключаем 
 servo1.write(90);
 }
//-------------------------------------------------------------Модуль флешка, ручка-замыкатор-----------------------------------------------
  
}




// Records the timing of knocks.
void listenToSecretKnock(){
  //Serial.println("knock starting");   

  int i = 0;
  // First lets reset the listening array.
  for (i=0;i<maximumKnocks;i++){
    knockReadings[i]=0;
  }
  
  int currentKnockNumber=0;                     // Incrementer for the array.
  int startTime=millis();                       // Reference for when this knock started.
  int now=millis();
  
  digitalWrite(led_pin2, LOW);                  // we blink the LED for a bit as a visual indicator of the knock.
  if (programButtonPressed==true){
     digitalWrite(led_pin3, LOW);                         // and the red one too if we're programming a new knock.
  }
  delay(knockFadeTime);                                 // wait for this peak to fade before we listen to the next one.
  digitalWrite(led_pin2, HIGH);  
  if (programButtonPressed==true){
     digitalWrite(led_pin3, HIGH);                        
  }
  do {
    //listen for the next knock or wait for it to timeout. 
    knockSensorValue = analogRead(knockSensor);
    if (knockSensorValue >=threshold){                   //got another knock...
      //record the delay time.
      Serial.println("knock.");
      now=millis();
      knockReadings[currentKnockNumber] = now-startTime;
      currentKnockNumber ++;                             //increment the counter
      startTime=now;          
      // and reset our timer for the next knock
      digitalWrite(led_pin2, LOW);  
      if (programButtonPressed==true){
        digitalWrite(led_pin3, LOW);                       // and the red one too if we're programming a new knock.
      }
      delay(knockFadeTime);                              // again, a little delay to let the knock decay.
      digitalWrite(led_pin2, HIGH);
      if (programButtonPressed==true){
        digitalWrite(led_pin3, HIGH);                         
      }
    }

    now=millis();
    
    //did we timeout or run out of knocks?
  } while ((now-startTime < knockComplete) && (currentKnockNumber < maximumKnocks));
  
  //we've got our knock recorded, lets see if it's valid
  if (programButtonPressed==false){             // only if we're not in progrmaing mode.
    if (validateKnock() == true){
      triggerDoorUnlock(); 
    } else {
      //Serial.println("Secret knock failed.");
      digitalWrite(led_pin2, LOW);          // We didn't unlock, so blink the red LED as visual feedback.
      for (i=0;i<4;i++){                    
        digitalWrite(led_pin2, HIGH);
        delay(100);
        digitalWrite(led_pin2, LOW);
        delay(100);
      }
      digitalWrite(led_pin2, HIGH);
    }
  } else { // if we're in programming mode we still validate the lock, we just don't do anything with the lock
    validateKnock();
    // and we blink the green and red alternately to show that program is complete.
    //Serial.println("New lock stored.");
    digitalWrite(led_pin3, LOW);
    digitalWrite(led_pin2, HIGH);
    for (i=0;i<3;i++){
      delay(100);
      digitalWrite(led_pin3, HIGH);
      digitalWrite(led_pin2, LOW);
      delay(100);
      digitalWrite(led_pin3, LOW);
      digitalWrite(led_pin2, HIGH);      
    }
  }
}


// Runs the motor (or whatever) to unlock the door.
void triggerDoorUnlock(){
  //Serial.println("Door unlocked!");
  int i=0;
  servo3.write(90);
  // turn the motor on for a bit.
  digitalWrite(led_pin2, LOW);            // And the green LED too.
  
  // Blink the green LED a few times for more visual feedback.
  for (i=0; i < 15; i++){   
      digitalWrite(led_pin3, LOW);
      delay(300);
      digitalWrite(led_pin3, HIGH);
      delay(300);
  }
   
}

// Sees if our knock matches the secret.
// returns true if it's a good knock, false if it's not.
// todo: break it into smaller functions for readability.
boolean validateKnock(){
  int i=0;
 
  // simplest check first: Did we get the right number of knocks?
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;                     // We use this later to normalize the times.
  
  for (i=0;i<maximumKnocks;i++){
    if (knockReadings[i] > 0){
      currentKnockCount++;
    }
    if (secretCode[i] > 0){                     //todo: precalculate this.
      secretKnockCount++;
    }
    
    if (knockReadings[i] > maxKnockInterval){   // collect normalization data while we're looping.
      maxKnockInterval = knockReadings[i];
    }
  }
  
  // If we're recording a new knock, save the info and get out of here.
  if (programButtonPressed==true){
      for (i=0;i<maximumKnocks;i++){ // normalize the times
        secretCode[i]= map(knockReadings[i],0, maxKnockInterval, 0, 100); 
      }
      savesecretCode();
      // And flash the lights in the recorded pattern to let us know it's been programmed.
      digitalWrite(led_pin2, LOW);
      digitalWrite(led_pin3, LOW);
      delay(1000);
      digitalWrite(led_pin2, HIGH);
      digitalWrite(led_pin3, HIGH);
      delay(50);
      for (i = 0; i < maximumKnocks ; i++){
        digitalWrite(led_pin2, LOW);
        digitalWrite(led_pin3, LOW);  
        // only turn it on if there's a delay
        if (secretCode[i] > 0){                                   
          delay( map(secretCode[i],0, 100, 0, maxKnockInterval)); // Expand the time back out to what it was.  Roughly. 
          digitalWrite(led_pin2, HIGH);
          digitalWrite(led_pin3, HIGH);
        }
        delay(50);
      }
      return false;     // We don't unlock the door when we are recording a new knock.
  }
  
  if (currentKnockCount != secretKnockCount){
    return false; 
  }
  
  /*  Now we compare the relative intervals of our knocks, not the absolute time between them.
      (ie: if you do the same pattern slow or fast it should still open the door.)
      This makes it less picky, which while making it less secure can also make it
      less of a pain to use if you're tempo is a little slow or fast. 
  */
  int totaltimeDifferences=0;
  int timeDiff=0;
  for (i=0;i<maximumKnocks;i++){ // Normalize the times
    knockReadings[i]= map(knockReadings[i],0, maxKnockInterval, 0, 100);      
    timeDiff = abs(knockReadings[i]-secretCode[i]);
    if (timeDiff > rejectValue){ // Individual value too far out of whack
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences/secretKnockCount>averageRejectValue){
    return false; 
  }
  
  return true;
  
} 

void loadsecretCode()
{
for ( byte i = 0; i < maximumKnocks; i++ ) { secretCode[i] = EEPROM.read(i); }  
}

void savesecretCode()
{
for ( byte i = 0; i < maximumKnocks; i++ ) { EEPROM.write(i, secretCode[i]); }  
}
//-----------------------------------------------------------------------Конец настройки Датчик стука---------------------------------------------------------------------






void clearData() 
{
  user=0;
  while(currentCommand != 0)
  {   // This can be used for any array size, 
    Data[currentCommand--] = 0; //clear for new data
  }
  return;
}
