#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int begin = 0;
const int set_watch = 1;
const int start_watch = 2;
const int stop_watch = 3;
const int set_count = 4;
const int count_down = 5;
const int stop_count = 6;

float timer,set_time;
int state,minute,sec,milsec;
int press,release,mode;
int press_D3 = 0,press_D4 = 0;
String res = "";


IRAM_ATTR void pressed_D3();
IRAM_ATTR void pressed_D4();

void return_main();
void setup() {
  state = begin;
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  lcd.init();                      
  lcd.backlight();
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(D3), pressed_D3, FALLING);
  attachInterrupt(digitalPinToInterrupt(D4), pressed_D4, FALLING);
  lcd.clear();
}

void loop() {
  if(state == begin){  // state BEGIN
    minute = 0;
    sec = 0;
    milsec = 0;
    lcd.setCursor(4,0);
    lcd.print(": stopwatch");
    lcd.setCursor(4,1);
    lcd.print(": timer");
    lcd.setCursor(0,mode);
    lcd.print("->");
    lcd.setCursor(0,!mode);
    lcd.print("  ");
    if(press_D3 == 1){
      delay(50);
      if(digitalRead(D3) == 0){
        if(mode == 0){
          state = set_watch;
        }else{
          state = set_count;
        }
      }
      press_D3 = 0;
      lcd.clear();
    }

    if(press_D4 == 1){
      mode = !mode;
      press_D4 = 0;
    }

  }else if(state == set_watch){ // state SET_WATCH
    res = "";
    minute = 0;
    sec = 0;
    timer = 0;
    milsec = 0;
    lcd.setCursor(0,0);
    if(minute < 10){
      res += '0';
    }
    res += String(minute) +":";
    String str = String(timer);
    if(timer < 10){
      lcd.print('0');
      res += '0'+str.substring(0,1)+":"+str.substring(2);
    }else{
      res += str.substring(0,2)+":"+str.substring(3);
    }
    lcd.setCursor(0, 0);
    lcd.print(res);
    lcd.setCursor(11, 0);
    lcd.print("  Set ");

    if(press_D3 == 1){
      delay(50);
      if(digitalRead(D3) == 0){
        press_D3 = 0;
        state = start_watch;
        set_time = millis();
      }

    }

    if(press_D4 == 1){
      if(digitalRead(D4) == 0){
        release = millis();
      }else{
        if(release-press >= 1000){
          state = begin;
          return_main();
        }
        press_D4 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }

  }else if(state == start_watch){ //state == START_WATCH
    res = "";
    timer = millis() - set_time;
    milsec = timer;
    sec = milsec / 1000;
    minute = sec / 60;
    milsec %= 1000;
    sec %= 60;
    minute %= 60;
    lcd.setCursor(0,0);
    if(minute < 10){
      res += '0';
    }
    res += String(minute) +":";
    String str = String(sec)+"."+String(milsec);
    if(sec < 10){
      lcd.print('0');
      res += '0'+str.substring(0,1)+":"+str.substring(2,4);
    }else{
      res += str.substring(0,2)+":"+str.substring(3,5);
    }
    lcd.setCursor(0, 0);
    lcd.print(res);
    lcd.setCursor(11, 0);
    lcd.print("  Run ");

    if(press_D3 == 1){
      // delay(50);
      if(digitalRead(D3) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          state = stop_watch;
          press_D3 = 0;
        }else{
          state = set_watch;
          press_D3 = 0;
          lcd.clear();
        }
      }
      Serial.println(String(release)+" : " +String(press));
    }
    if(press_D4 == 1){
      if(digitalRead(D4) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          lcd.setCursor(0, 1);
          lcd.print("Rec : "+res);
        }else{
          state = begin;
          return_main();
        }
        press_D4 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }

    delay(120);
  }else if(state == stop_watch){ // state == STOP_WATCH
    lcd.setCursor(11, 0);
    lcd.print("Pause");
    set_time = millis() - timer;
    if(press_D3 == 1){
      if(digitalRead(D3) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          state = start_watch;
          press_D3 = 0;
          milsec = 0;
          sec = 0;
          minute = 0;
        }else{
          state = set_watch;
          press_D3 = 0;
          lcd.clear();
        }
      }
      Serial.println(String(release)+" : " +String(press));
    }

    if(press_D4 == 1){
      if(digitalRead(D4) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          lcd.setCursor(0, 1);
          lcd.print("Rec : "+res);
        }else{
          state = begin;
          return_main();
        }
        press_D4 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }

  }else if(state == set_count){ //state == SET_COUNT
    lcd.setCursor(4,1);
    lcd.print("Set-time");
    lcd.setCursor(5,0);
    if(minute < 10)lcd.print('0');
    lcd.print(String(minute)+":");
    lcd.setCursor(8,0);
    if(sec < 10) lcd.print('0');
    lcd.print(String(sec));

    if(press_D3 == 1){
      if(digitalRead(D3) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          state = count_down;
          set_time = millis();
        }else{
          
          minute = 0;
          sec = 0;
        }
        press_D3 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }

    if(press_D4 == 1){
      if(digitalRead(D4) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          minute += 1;
        }else{
          state = begin;
          return_main();
        }
        press_D4 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }
  }else if(state == count_down){ //state == RUN_COUNT_DOWN
    lcd.setCursor(5,0);
    if(minute < 10){
      lcd.print('0');
    }
    lcd.print(String(minute)+":");
    lcd.setCursor(8,0);
    if(sec < 10) lcd.print('0');
    lcd.print(String(sec));

    if(minute == 0 && sec == 0){
      minute = 0;
      sec = 0;
      lcd.setCursor(5,0);
      if(minute < 10){
        lcd.print('0');
      }
      lcd.print(String(minute)+":");
      lcd.setCursor(8,0);
      if(sec < 10) lcd.print('0');
      lcd.print(String(sec));
      delay(300);
      state = set_count;
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Set your time");
      delay(500);
      lcd.clear();
    }
    else if(minute > 0 || sec >= 0){
      lcd.setCursor(4,1);
      lcd.print("Running    ");
      if(millis()-set_time > 1000){ //-timer
        if(sec == 0){
          sec = 60;
          minute -= 1;
        }
        sec -= 1;
        set_time = millis();
      }
    }

    if(press_D3 == 1){
      if(digitalRead(D3) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          state = stop_count;
          set_time = millis();
        }else{
          minute = 0;
          sec = 0;
          state = set_count;
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Set your time");
          delay(500);
          lcd.clear();
          
        }
        press_D3 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }

    if(press_D4 == 1){
      if(digitalRead(D4) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){

        }else{
          state = begin;
          return_main();
        }
        press_D4 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }
  }else if(state == stop_count){ //state == STOP_COUNT
    lcd.setCursor(4,1);
    lcd.print(" Pause    ");
    if(press_D3 == 1){
      if(digitalRead(D3) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){
          state = count_down;
          set_time = millis();
        }else{
          minute = 0;
          sec = 0;
          state = set_count;
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Set your time");
          delay(500);
          lcd.clear();
          
        }
        press_D3 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }

    if(press_D4 == 1){
      if(digitalRead(D4) == 0){
        release = millis();
      }else{
        if(release-press <= 1000){

        }else{
          state = begin;
          return_main();
        }
        press_D4 = 0;
      }
      Serial.println(String(release)+" : " +String(press));
    }
  }
}


IRAM_ATTR void pressed_D3(){
  press_D3 = 1;
  press = millis();
}

IRAM_ATTR void pressed_D4(){
  press_D4 = 1;
  press = millis();
}

void return_main(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("return to main");
  delay(500);
  lcd.clear();  
  mode = 0;
}