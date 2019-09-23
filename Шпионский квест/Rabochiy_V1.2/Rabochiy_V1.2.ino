const int led = 13;
#define SensorPin 0
int isd = 1;
int soundPin=2; // контакт подключения выхода OUT датчика
// int relayPin=13; // контакт подключения выхода реле
boolean statuslamp; // состояние лампы true - включено, false - выключено

void setup() {
  pinMode( led, OUTPUT );
  pinMode(SensorPin, INPUT);
  pinMode(isd, OUTPUT);
//pinMode(relayPin,OUTPUT); // настройка вывода реле в режим OUTPUT
statuslamp=false; // начальное состояние - лампа выключена
//digitalWrite(relayPin,LOW);
}

void loop() {

 //----------------------------------------------------------------Датчик препятствия и воспроизведение звука----------------------------
int SensorVal = digitalRead(SensorPin);
  //Если обнаружили движение включаем последовательно реле
  if(SensorVal == LOW)
  {
    digitalWrite(isd, HIGH); //Включаем реле 1
    digitalWrite(led, HIGH);
    delay(4000); //Задаем пауз между включением первого реле и второго
    digitalWrite(isd, LOW); //Выключаем реле 1
    digitalWrite(led, LOW);
    delay(2000); //Задаем пауз между выключением первого реле и второго
   }

// ----------------------------------------------------------------------Датчик звука---------------------------------------------------------------------
if(digitalRead(soundPin)==0) // проверяем значение на выводе OUT датчика звука
{
statuslamp=!statuslamp; // поменять статус лампы
digitalWrite(led,statuslamp); // переключить лампу
delay(10);
}

}
