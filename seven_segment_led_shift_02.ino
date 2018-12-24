/*
  74HC595 -> Arduino/LED
  1 Q1         LED2(D)
  2 Q2         LED3(DP)
  3 Q3         LED4(C)
  4 Q4         LED5(G)
  5 Q5         LED7(B)
  6 Q6         LED10(F)
  7 Q7         LED11(A)
  8 GND        GND
  9 Q7S        OPEN
  10 MR        +5V
  11 SHCP      13 SCK
  12 STCP      10 SS
  13 OE        GND
  14 DS        11 MOSI
  15 Q0        LED1(E)
  16 VCC       +5V
*/

#include "SPI.h"

const int cathode_pins[] = {2, 3, 4, 5};  // カソードに接続するArduinoのピン
const int number_of_cathode_pins = sizeof(cathode_pins) / sizeof(cathode_pins[0]);
int numbers_to_display = 0; // LEDに表示する数字を保持する変数

const byte digits[] = {
  0b11101011, // 0
  0b00101000, // 1
  0b10110011, // 2
  0b10111010, // 3
  0b01111000, // 4
  0b11011010, // 5
  0b11011011, // 6
  0b11101000, // 7
  0b11111011, // 8
  0b11111010, // 9
};

// 1桁の数字(n)を表示する
void display_number (int n) {
  digitalWrite(SS, LOW);
  SPI.transfer(digits[n]);
  digitalWrite(SS, HIGH);
}

// アノードをすべてLOWにする
void clear_segments() {
  digitalWrite(SS, LOW);
  SPI.transfer(0);
  digitalWrite(SS, HIGH);
}

void display_numbers () {
  int n = numbers_to_display;  // number_to_displayの値を書き換えないために変数にコピー
  for (int i = 0; i < number_of_cathode_pins; i++) {
    digitalWrite(cathode_pins[i], LOW);
    display_number(n % 10); // 最後の一桁を表示する
    delayMicroseconds(100);
    clear_segments();
    digitalWrite(cathode_pins[i], HIGH);
    n = n / 10; // 10で割る
  }
}

void set_numbers(int n) {
  noInterrupts();
  numbers_to_display = n;
  interrupts();
}

void setup() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  
  for (int i = 0; i < number_of_cathode_pins; i++) {
    pinMode(cathode_pins[i], OUTPUT);  // cathode_pinを出力モードに設定する
    digitalWrite(cathode_pins[i], HIGH);
  }

  // f = クロック周波数 / ( 2 * 分周比　*　( 1 + 比較レジスタの値))
  // 分周比=32, 比較レジスタの1値=255 -> f = 16000000 / (2 * 32 * 256) = 976 Hz
  OCR2A = 255; // 255クロックごとに割り込みをかける
  TCCR2B = 0b100; // 分周比を32に設定する
  bitWrite(TIMSK2, OCIE2A, 1); // TIMER2を
}

void loop () {
  for (int i = 0; i < 10000; i++) {
    set_numbers(i);
    delay(100);
  }
}

ISR(TIMER2_COMPA_vect)
{
  display_numbers();
}

