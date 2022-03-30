#include <MsTimer2.h>
#include <Wire.h>


const int SLAVE_ADDRESS = 0x14;
const char COMM_RELAY = 120;//'c'
volatile char comm;
volatile bool blink = false;

int pin_pul = 8;
int pin_dir = 9;
int pin_connectA = 10;
int pin_connectB = 11;
bool connectAB=false;
const int tempTimer = 100;//период таймера, мс.

const int MAX_TIME_WHEEL = 500;//макс время поворота, мс.
volatile int pulsTime = 0;// <0 : влево, >0 : вправо, =0 не поворачиваем.
volatile int currTime = 0;// текущее время поворота (не больше MAX_TIME_WHEEL)
volatile bool hl = true;

void setup() {
  //-------------pin driver and relay------------------
  pinMode(pin_pul, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_connectA, OUTPUT);
  pinMode(pin_connectB, OUTPUT);
  digitalWrite(pin_connectA, HIGH);
  digitalWrite(pin_connectB, HIGH);

  //-------------Timer----------------------------------
  MsTimer2::set(tempTimer, timerInterupt); // задаем период(tempTimer) прерывания по таймеру
  MsTimer2::start(); // разрешаем прерывание по таймеру

  //-------------test blink-----------------------------
  pinMode(LED_BUILTIN, OUTPUT);//test

  //-------------I2C slave init-------------------------
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(processMessage);
  Wire.onRequest(processAnsver);
}

void loop() {
  //================================================
  if (blink) {
    digitalWrite(LED_BUILTIN, HIGH);//////////////
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);///////////////
  }
  //================================================
}

void timerInterupt() {
  //если время вращения не нулевое
  if (pulsTime != 0) {
    
    //если оно меньше 0, проверяем текущий поворот на 
    //не превышение максимального и в случае истины
    //отправляем сигнал на поворот вправо и повышаем
    //счетчик текущего поворота
    if (pulsTime < 0) {
      if(currTime < MAX_TIME_WHEEL) {
        digitalWrite(pin_dir, HIGH);
        if (hl) currTime += tempTimer;
      }
    //в лубом случае приблихаем время вреащения к 0 
    if (hl)pulsTime += tempTimer;

    //если оно больше 0, проверяем текущий поворот на 
    //не превышение максимального и в случае истины
    //отправляем сигнал на поворот влево и уменьшаем
    //счетчик текущего поворота
    } else {
      if(currTime >-MAX_TIME_WHEEL) {
        digitalWrite(pin_dir, LOW);
        if (hl) currTime -= tempTimer;
      }
      
      if (hl)pulsTime -= tempTimer;
    }

    //если текущий счетчик поворота не превышает 
    //макимальный поворот отправляем сигнал на поворот
    if(currTime < MAX_TIME_WHEEL && currTime >-MAX_TIME_WHEEL) {
      if (hl) {
        digitalWrite(pin_pul, HIGH);
      }
      else {
        digitalWrite(pin_pul, LOW);
      }
    }

    hl = !hl;

//==================================
    //blink = !blink;
//==================================

  //если время вращения стала 0 и счетчик вращения не пуст
  //уменьшаем его (или увеличиваем) в обратном направлении
  //укаывая попутно направление вращения
  } else if(currTime != 0) {
    if(currTime > 0) {
      digitalWrite(pin_dir, LOW);
      if (hl) currTime -= tempTimer;
    } else {
      digitalWrite(pin_dir, HIGH);
      if (hl) currTime += tempTimer;
    }

    //и отправляем сигнал на поворот, пока счетчик не =0
    if (hl) {
      digitalWrite(pin_pul, HIGH);
    }
    else {
      digitalWrite(pin_pul, LOW);
    }

    hl = !hl;
  }
}

void processMessage(int n) {
  if(Wire.available()) {
    comm = Wire.read();

    //если команта на переклчение реле
    if (comm == COMM_RELAY)
    {
      connectAB=!connectAB;
      if(connectAB) {
        digitalWrite(pin_connectA, LOW);
        digitalWrite(pin_connectB, LOW);

        blink = true;/////////////////////////////////
      } else {
        digitalWrite(pin_connectA, HIGH);
        digitalWrite(pin_connectB, HIGH);
        
        blink = false;////////////////////////////////
      }

      //если команта на вращение
    } else if (comm >= 0 && comm <= 100) {
        //если отсутствует текущая команта и счеткик пуст
        if(pulsTime==0 && currTime==0) {
          int t = comm*100 - 5000;//время поворота в мс
          pulsTime = t;
        }
    } //else if() {......}
    //-------
  }
}

void processAnsver() {
  //if(comm == 'g') {
    //Wire.write(puls);
  //}
  //if(comm == 'f') {
    //Wire.write(puls);
  //}
  
}
