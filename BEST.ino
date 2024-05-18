#include <SPI.h>
#include <SD.h>
#include <UnixTime.h>                          // подключение библиотеки
#include <Wire.h>
//#include <avr/io.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C  lcd(0x27,16,2); // 0x27 is the I2C bus address for an unmodified backpack
#include <iarduino_RTC.h>                               // подключение библиотеки
iarduino_RTC time(RTC_DS1302,8,6,7); 
UnixTime stamp(0);  // указать GMT (4 для Ижевска)
const int relayPin = 10; // Пин, к которому подключено реле
const byte MAX_ROWS = 6; // Максимальное количество строк в файле
const byte MAX_COLS = 2;   // Максимальное количество столбцов в каждой строке
String data[MAX_ROWS][MAX_COLS]; // Массив для хранения данных
long mintime = 9999999;
long RazTime = 9999999;
File myFile;
String VremZV;

word LCD_Buttons_ADC;
char myStr [5];
int y = 0;
char v = "a";
boolean flag1=false;
void setup()

{Serial.begin(9600);
{ pinMode(relayPin, OUTPUT); // Устанавливаем пин реле как выход
  digitalWrite(relayPin, HIGH); // Выключаем реле
   while (!Serial) 
   
  {
    ; // Ожидаем подключение монитора последовательного порта
  }
   }
   if (!SD.begin(4)) {
    Serial.println(PSTR("Ошибка инициализации SD-карты"));
    return;
  }
    myFile = SD.open("zvonok.csv", FILE_READ); // Открытие файла для чтения
  if (myFile) {
    int row = 0;
    while (myFile.available()) {
      String line = myFile.readStringUntil('\n'); // Чтение строки из файла
      int col = 0;
      int startIndex = 0;
      int endIndex = 0;
      // Разбиение строки на элементы, используя разделитель ';'
      while ((endIndex = line.indexOf(';', startIndex)) != -1 && col < MAX_COLS) {
        data[row][col++] = line.substring(startIndex, endIndex);
        startIndex = endIndex + 1;
      }
      // Добавление последнего элемента строки в массив данных
      if (col < MAX_COLS) {
        data[row][col] = line.substring(startIndex);
      }
      row++;
    }
    myFile.close();

    // Вывод данных
    for (int i = 0; i < row; i++) {
      for (int j = 0; j < MAX_COLS; j++) {
       // Serial.print(data[i][j]);
        if (j < MAX_COLS - 1) {
         // Serial.print(" ");
        }
      }
      //Serial.println();
    }

    // Вывод значения из второй строки и четвертого столбца
   // Serial.print(data[1][3]); // Вывести значение из 2 строки и 4 столбца (индексация начинается с 0)
  } else {
  //  Serial.println("Ошибка открытия файла");
  }
   // 00 сек, 30 мин, 12 час, 25 число, октябрь, 2022 год, вторник
   
lcd.init();
lcd.begin(16,2);
//lcd.setCursor(0,0);                     // Установить курсор на первую строку  
//lcd.print("LCD1602");                   // Вывести текст
//lcd.setCursor(0,1);                     // Установить курсор на вторую строку
//lcd.print("www.robotchip.ru"); 

time.begin();
// 00 сек, 30 мин, 12 час, 25 число, октябрь, 2022 год, вторник
//time.settime(20, 33, 22, 14, 05, 24, 2);   // ставим только ниже begin!!!!!!!!!!!!!!!!!!!!!!
delay(10);
}
void loop() {
  
   int x;
   v= 'a';
String Data;
   long realtimeunix = time.gettimeUnix();
 // delay(1000);
  Data = time.gettime("d.m.Y H:i:s"); // Получение текущего времени
  
if (flag1 == false){
  for (int i = 1; i < MAX_ROWS; i++){

String str11 = (data[i][1]); 
// Заменяем символы на запятые
str11.replace(".", ",");
str11.replace(" ", ",");
str11.replace(":", ",");

// Добавляем ".00" в конец строки
str11 += ",00";

//Serial.println(str11);
// Разделяем строку на составляющие
int day = str11.substring(0, 2).toInt();
int month = str11.substring(3, 6).toInt();
int year = str11.substring(6, 8).toInt()+2000;
int hour = str11.substring(9, 11).toInt();
int minute = str11.substring(12, 14).toInt();
int second = str11.substring(15, 17).toInt();
// Выполняем функцию с полученными аргументами
//Serial.print("Время файла: ");
//Serial.print(i);
//Serial.print(" ");
//Serial.print(str11);
//Serial.print(" ");
//Serial.println(minute);
 stamp.setDateTime(year, month, day, hour, minute, second);   // 20 мая 2021, 7:04:15
//Serial.println(myFunc(day, month, year, hour, minute, second));
//Serial.print("Время реальное: "); Serial.println(realtimeunix);
 uint32_t unix = stamp.getUnix();
realtimeunix = time.gettimeUnix();
 //delay(100);
 //Serial.println(unix);
//Serial.print("timef: ");Serial.print(i);Serial.print(" ");Serial.print(unix);Serial.print(" ");  Serial.println (data[i][1]);
  //if ((unix - realtimeunix) > 0)
  if (unix > realtimeunix)
    {//Serial.println ("сработал");
    RazTime = (unix - realtimeunix);
    if (RazTime < mintime){
      mintime=RazTime;
      VremZV = data[i][1];
     // VremZV = VremZV.substring(0, VremZV.length() - 3);
      VremZV.replace(".05.24", "");
      Serial.println(mintime);
    }}
 lcd.setCursor(1,1);          //  задержка в одну секунду перед выводом времени в последовательный порт
    lcd.print (time.gettime("d-m H:i:s"));
    lcd.setCursor(10,0);
    lcd.print ("         ");
    lcd.setCursor(10,0);
    lcd.print (mintime);
    lcd.setCursor(0,0);
    lcd.print (VremZV);
  if (mintime < 3){
  //Serial.println("Включение реле..."); // Выводим сообщение о включении реле
  
  digitalWrite(relayPin, LOW); // Включаем реле
  mintime = 99999;
  RazTime= 99999;
 delay(5000); // Задержка на 5 секунд (5000 миллисекунд)
  //Serial.println("Выключение реле..."); // Выводим сообщение о выключении реле
  digitalWrite(relayPin, HIGH); // Выключаем реле
  //delay (1000);
   }}

}





   //Serial.println(y);
  if (flag1 == false) {
   if(millis()%10==0) { 
  //  lcd.setCursor(1,1);          //  задержка в одну секунду перед выводом времени в последовательный порт
  //  lcd.print (time.gettime("d-m H:i:s"));
 //     Serial.println(time.gettime("d-m-Y, H:i:s, D"));   // выводим время
      delay(1);  // делаем небольшую задержку, чтобы не выводить время слишком часто 
    }                                  // Создаем переменную x
  }
  x = analogRead (0);                     // Задаем номер порта с которого производим считывание 
  lcd.setCursor(10,1);                    // Установить курсор на вторую строку 
  if (x < 100) {                          // Если x меньше 100 перейти на следующею строк
  v= 'R';
 // lcd.print ("Right ");                   // Вывести текст 
  //Serial.print("Value A0 ‘Right’ is  :"); // Вывести текст
  //Serial.println(x,DEC);                  // Вывести значение переменной x;
}
  else if (x < 200) {                     // Если х меньше 200 перейти на следующию строку
  v = 'U';
  //lcd.print ("Up    ");                   // Вывести текст 
 // Serial.print("Value A0 ‘UP’ is  :");    // Вывести текст
 // Serial.println(x,DEC);                  // Вывести значение переменной x
}
  else if (x < 400){                      // Если х меньше 400 перейти на следующию строку
  v= 'D';
  //lcd.print ("Down  ");                   // Вывести текст
  //Serial.print("Value A0 ‘Down’ is  :");  // Вывести текст
 // Serial.println(x,DEC);                  // Вывести значение переменной x
}
  else if (x < 600){                      // Если х меньше 600 перейти на следующию строку
  v= 'L';
  //lcd.print ("Left  ");                   // Вывести текст
 // Serial.print("Value A0 ‘Left’ is  :");  // Вывести текст
  //Serial.println(x,DEC);                  // Вывести значение переменной x
}
  else if (x < 800){                      // Если х меньше 800 перейти на следующию строку
  v = 'S';
 // lcd.print ("Select");                   // Вывести текст
  //Serial.print("Value A0 ‘Select’ is  :");// Вывести текст
 // Serial.println(x,DEC);                  // Вывести значение переменной x
  flag1=true;
 /* lcd.clear();
  if (y == 0){
    lcd.clear();
    lcd.print("menu");
  }
  delay(300);
  
  if (y == 1){
    lcd.clear();
    lcd.print("save files");
    
    Serial.println(y);
  }
  if (y == 2){
    lcd.clear();
    lcd.print("read files");
    
    Serial.println(y);
  }
  if (y == 3){
    lcd.clear();
    lcd.print("change time");
    
    Serial.println(y);
  }
  */
 // y= y + 1;
//  if (y == 4) {y = 0;}
}
if (flag1 == true){

 Serial.println("y= ");
 Serial.println(y);
 Serial.println("v= ");
 Serial.println(v);
 if (v == 'U') {y=y + 1;}
 //v = 'a';
 delay(100);
 if (v == 'D') {y=y - 1;}
 delay(100);
 if (y == 4 || y ==-4 ) {y = 0;} {
 if (y== 0) {
  lcd.clear();
  lcd.print("menu");
 }
 if (y == 1 || y == -3) {
  lcd.clear();
  lcd.print("save files");
 }
 if (y == 2 || y == -2) {
  lcd.clear();
  lcd.print("read files");
 }
 if (y == 3 || y == -1) {
  lcd.clear();
  lcd.print("change time");
 }
 }
 }
//if (600 <= x && 800 >= x) {lcd.print}
}