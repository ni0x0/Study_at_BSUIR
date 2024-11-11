#include "nRF24L01.h" // библиотека для nRF24L01+
#include "RF24.h"     // библиотека для радио модуля

#include <Adafruit_PWMServoDriver.h> // библиотека для PWM модуля
#include <LiquidCrystal_I2C.h>       // библиотека для дисплея

#define PIPE_ID 0xF0F1F2F3F4LL  // идентификатор передачи
#define PIN_CE  9               // номер вывода, к которому подключен вывод CE радиомодуля
#define PIN_CSN 10              // номер вывода, к которому подключен вывод CSN радиомодуля
RF24 radio(PIN_CE, PIN_CSN);    // создание объекта radio с указанием выводов CE и CSN
#define RADIO_CHANNEL 0         // используемый канал (м.б. установлен от 0 до 127) - 2,4 ГГц

#define PIPE_ID_STR "0xF0F1F2F3F4"
#define RADIO_FREQ_STR "2.4GHz"
#define RADIO_SPEED_STR "1MBPS"

#define AXIS_NUM 6       // количество осей вращения
uint8_t data[AXIS_NUM];  // массив для хранения полученных данных

// создание объекта pwm1 для управления сервоприводами с помощью pca9685
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(); 
#define START_SERVO_045 310    // значения для установления начальных позиций сервоприводов
#define START_SERVO_12 340     
#define START_SERVO_3 390

// частота работы модуля
#define PCA_FREQ 50 
// момент цикла из 4096 частей для установления низкого уровня ШИМ, соответствующий установке сервопривода на угол поворота 0°
#define PWM_MIN_BOUND 100      
// момент цикла из 4096 частей для установления низкого уровня ШИМ, соответствующий установке сервопривода на угол поворота 180°
#define PWM_MAX_BOUND 520

#define MIN_DEGREE 0         // минимальный возможный угол поворота сервопривода
#define MAX_DEGREE 180       // максимальный возможный угол поворота сервопривода

#define AXIS_1_MIN_BOUND 50   // минимальный угол поворота на оси 1
#define AXIS_2_MAX_BOUND 140  // максимальный угол поворота на оси 2
#define AXIS_5_MIN_BOUND 50   // минимальный угол поворота на оси 5
#define AXIS_5_MAX_BOUND 140  // максимальный угол поворота на оси 5

#define LCD_I2C_ADDRESS 0x27  // I2C адрес дисплея
#define LCD_COLUMS_NUM 16     // количество столбцов в дисплее
#define LCD_ROWS_NUM 2        // количество строк в дисплее
// создание объекта для управления дисплеем с помощью I2C интерфейса
LiquidCrystal_I2C lcd(0x27,16,2); 

#define SWITCH1_PIN 3         // номер вывода, к которому подключена первая кнопка
#define SWITCH2_PIN 4         // номер вывода, к которому подключена вторая кнопка
boolean lastButton_1 = LOW;   // предыдущее состояние первой кнопки
boolean lastButton_2 = LOW;   // предыдущее состояние второй кнопки
boolean currButt_1 = LOW;     // текущее состояние первой кнопки
boolean currButt_2 = LOW;     // текущее состояние второй кнопки

#define LED_PIN_GREEN 5       // номер вывода, к которому подключен зеленый светодиод
#define LED_PIN_YELLOW 6      // номер вывода, к которому подключен желтый светодиод
#define LED_PIN_BLUE 7        // номер вывода, к которому подключен синий светодиод
boolean ledOn_green = false;  // состояние зеленого светодиода
boolean ledOn_blue = false;   // состояние синего светодиода

#define BLINKS_NUM 2
#define BLINK_ON 250
#define BLINK_OFF 380

// прием данных разрешен/запрещен
boolean data_receiving_is_on = true;
// на дисплей выводятся углы установки сервоприводов
boolean lcd_vals_mode_is_on = true;   

// Функция установки значений и параметров
void setup() {

  // РАДИОМОДУЛЬ
  radio.begin();                     // инициализация модуля NRF24L01
  radio.setChannel(RADIO_CHANNEL);   // выбираем канал
  radio.setDataRate(RF24_1MBPS);     // скорость передачи данных 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH);    // устанавливаем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe(1, PIPE_ID); // открываем одну трубу с уникальным ID
  radio.startListening();            // начинаем прослушивать трубу

  // ШИМ МОДУЛЬ
  pwm1.begin();                 // инициализация I2C интерфейса для 16-тиканального ШИМ модуля
  pwm1.setPWMFreq(PCA_FREQ);    // устанавливаем частоту 

  pwm1.setPWM(0, 0, START_SERVO_045);       // установка начальных положений сервоприводов
  pwm1.setPWM(1, 0, START_SERVO_12);        // 1-й параметр: выбор сервопривода 
  pwm1.setPWM(2, 0, START_SERVO_12);        // 2-й параметр: на какой момент цикла из 4096 частей задать активный уровень ШИМ
                                            // 3-й параметр: на какой момент цикла из 4096 частей задать низкий уровень ШИМ
  pwm1.setPWM(3, 0, START_SERVO_3);                     
  pwm1.setPWM(4, 0, START_SERVO_045);
  pwm1.setPWM(5, 0, START_SERVO_045);

  // ДИСПЛЕЙ
  lcd.init();                     // инициализация I2C интерфейса для дисплея
  lcd.backlight();                // включение подсветки дисплея
  lcd.print("Hell");

  // ВЫВОДЫ
  pinMode(SWITCH1_PIN, INPUT);        // установка режима ввода для кнопок 
  pinMode(SWITCH2_PIN, INPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);     // установка режима вывода для светодиодов
  pinMode(LED_PIN_YELLOW, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);

  // Мигание желтым светодиодом в начале работы устройства
  int i = 0;
  for (; i < BLINKS_NUM; i++) {
    digitalWrite(LED_PIN_YELLOW, HIGH);
    delay(BLINKS_ON);
    digitalWrite(LED_PIN_YELLOW, LOW);
    delay(BLINKS_OFF);
  }
  digitalWrite(LED_PIN_YELLOW, HIGH);
}

// Функция вывода на дисплей полученных по радиоканалу чисел
void print_degrees() {
  int i = 0;
  int j = 0;
  for (; i < AXIS_NUM; i++) {
    if (i % 2 == 0)
      lcd.setCursor(j, 0);
    else {
     lcd.setCursor(j, 1); 
     j += 5;
    } 
    lcd.print(i + 1);
    lcd.print(":");
    lcd.print(data[i]);
    lcd.print(" ");
  } // for (; i < AXIS_NUM; i++)
}

// Функция для обработки нажатия кнопки (с учетом дребезжания)
boolean debounce(boolean last, int swPin) {
  boolean current = digitalRead(swPin);
  if (last != current) {
    delay(5);
    current = digitalRead(swPin);
  }
  return current;
}

// Функция вывода настроек радиомодуля
void print_radio_info() {
  lcd.setCursor(0, 0);
  lcd.print("ID:");
  lcd.print(PIPE_ID_STR);

  lcd.setCursor(0, 1);
  lcd.print("F-");
  lcd.print(RADIO_FREQ_STR);

  lcd.setCursor(9, 1);
  lcd.print("v-");
  lcd.print(RADIO_SPEED_STR);
}

// Главная фунция, зацикленно выполняющаяся устройством
void loop() {
  
  // Проверка разрешения на прием данных
  if (data_receiving_is_on == true) {

    // Проверяем буфер обмена
    if (radio.available()) {

      radio.read(&data, sizeof(data)); // читаем данные из канала
      
      digitalWrite(LED_PIN_BLUE, HIGH);

      if (lcd_vals_mode_is_on == true)
        print_degrees();

      // Установка углов поворотов сервоприводов с учетом заданных углов ограничений
      pwm1.setPWM(0, 0, map(data[0], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));

      if (data[1] > AXIS_1_MIN_BOUND) {
        pwm1.setPWM(1, 0, map(data[1], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));
        pwm1.setPWM(2, 0, map(data[1], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));
      }

      if (data[2] < AXIS_2_MAX_BOUND)
        pwm1.setPWM(3, 0, map(data[2], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));

      pwm1.setPWM(4, 0, map(data[3], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));

      pwm1.setPWM(5, 0, map(data[4], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));

      if (data[5] > AXIS_5_MIN_BOUND && data[5] < AXIS_5_MAX_BOUND)
        pwm1.setPWM(6, 0, map(data[5], MIN_DEGREE, MAX_DEGREE, PWM_MIN_BOUND, PWM_MAX_BOUND));
      
        
      delay(20);
    } //if (radio.available()) 
    else 
      digitalWrite(LED_PIN_BLUE, LOW);

  } // if (data_receiving_is_on == true)
  else 
    digitalWrite(LED_PIN_BLUE, LOW);
  

  // Обрабатываем нажатие кнопки смены выводимой на экран информации
  currButt_1 = debounce(lastButton_1, SWITCH1_PIN);
  if (lastButton_1 == LOW && currButt_1 == HIGH){

    lcd_vals_mode_is_on = !lcd_vals_mode_is_on;
    if (lcd_vals_mode_is_on == false) 
      print_radio_info();
  }
  lastButton_1 = currButt_1;
  

  currButt_2 = debounce(lastButton_2, SWITCH2_PIN);
  if (lastButton_2 == LOW && currButt_2 == HIGH){
    data_receiving_is_on = !data_receiving_is_on;
  }
  lastButton_2 = currButt_2;
  digitalWrite(LED_PIN_GREEN, data_receiving_is_on);

  delay(10);
}







