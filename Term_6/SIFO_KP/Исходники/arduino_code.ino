#include <AccelStepper.h>
#include<EEPROM.h>

#define SM1_IN1 9
#define SM1_IN2 10
#define SM1_IN3 11
#define SM1_IN4 12

#define SM2_IN1 5
#define SM2_IN2 6
#define SM2_IN3 7
#define SM2_IN4 8

#define YELL_L 13
#define GREEN_L A1
#define BLUE_L A2

#define HOLL_PIN 4
#define BUTT_PIN 3

#define HORIS_PHOTOS_NUM 5

#define DEGRS90 4075
#define DEGRS72 3668
#define DEGRS45 2036
#define DEGRS60 2716


AccelStepper step_motor1(8, SM1_IN1, SM1_IN3, SM1_IN2, SM1_IN4);
AccelStepper step_motor2(8, SM2_IN1, SM2_IN3, SM2_IN2, SM2_IN4);

boolean lastButton = LOW;
boolean blLedOn = false;
boolean currButton = LOW;

boolean need_to_find_start_pos = true;
boolean need_to_make_360_review = false;

int address = 0;
byte photo_value = 1;
String file_template = "_photo_";
String file_extension = ".jpg";

void setup(){
  Serial.begin(9600);

  pinMode(YELL_L, OUTPUT);
  pinMode(GREEN_L, OUTPUT);
  pinMode(BLUE_L, OUTPUT);

  digitalWrite(GREEN_L, HIGH);

  pinMode(HOLL_PIN, INPUT);
  pinMode(BUTT_PIN, INPUT);

  step_motor1.setMaxSpeed(900); 
  step_motor1.setAcceleration(200); 
  step_motor1.setSpeed(800);

  step_motor2.setMaxSpeed(900); 
  step_motor2.setAcceleration(200); 
  step_motor2.setSpeed(800);
}


boolean debounce(boolean last) {
  boolean current = digitalRead(BUTT_PIN);
  if (last != current) {
    delay(5);
    current = digitalRead(BUTT_PIN);
  }
  return current;
}


void find_start_position() {
  step_motor1.setSpeed(-800);

  while (true) {
    step_motor1.runSpeed();
    if (digitalRead(HOLL_PIN) == LOW)
      break;
  }

  step_motor1.stop();
  need_to_find_start_pos = false;
}

void take_n_steps(AccelStepper & stepper, long steps_num) {
  stepper.move(steps_num);
  while (true) {
    if (stepper.distanceToGo() == 0)
      break;
    stepper.run();
  }  
}

void photo_blink(int led_p) {
  digitalWrite(led_p, HIGH);
  delay(500);
  digitalWrite(led_p, LOW);
}

void make_360_review() { 
  photo_value = EEPROM.read(address);

  String file_name = String(photo_value) + file_template + "bottom" + file_extension;
  Serial.println(file_name);
  photo_blink(BLUE_L);
  take_n_steps(step_motor1, DEGRS90);

  for (int i = 0; i < HORIS_PHOTOS_NUM; i++) {
    file_name = String(photo_value) + file_template + "side_" + String(i) + file_extension;
    Serial.println(file_name);
    photo_blink(BLUE_L);
    take_n_steps(step_motor2, DEGRS72);
  }
  take_n_steps(step_motor1, DEGRS90);

  file_name = String(photo_value) + file_template + "up" + file_extension;
  Serial.println(file_name);
  photo_blink(BLUE_L);

  photo_value++;
  EEPROM.write(address, photo_value);
  need_to_make_360_review = false;
}


void loop(){
  if (need_to_find_start_pos == true) {
    digitalWrite(YELL_L, HIGH);  
    find_start_position(); 
    digitalWrite(YELL_L, LOW);
  }

  if (need_to_make_360_review == true)
    make_360_review();

  
  currButton = debounce(lastButton);
  if (lastButton == LOW && currButton == HIGH){

    need_to_find_start_pos = !need_to_find_start_pos;
    need_to_make_360_review = !need_to_make_360_review;
  }
  lastButton = currButton;  
}
