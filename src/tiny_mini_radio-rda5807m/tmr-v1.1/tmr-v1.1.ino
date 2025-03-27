//83% ai-code
//arduhelp
#include <Wire.h>

#define ENCODER_CLK PB1  // Пін для CLK енкодера
#define ENCODER_DT PB3   // Пін для DT енкодера
#define LED PB4          // Пін для LED
#define SCAN_BTN PB5     // Пін для кнопки сканування

float freq = 92.0;   // Початкова частота
const float FREQ_MIN = 87.5;
const float FREQ_MAX = 108.0;
byte scanned = 0;

int lastStateCLK;

// *** Функція встановлення частоти ***
void setFrequency(float frequency) {
  unsigned int frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  byte frequencyH = frequencyB >> 8;
  byte frequencyL = frequencyB & 0xFF;

  Wire.beginTransmission(0x60);
  Wire.write(frequencyH);
  Wire.write(frequencyL);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
}

// *** Обробка енкодера ***
void readEncoder() {
  int stateCLK = digitalRead(ENCODER_CLK);
  if (stateCLK != lastStateCLK) {  // Тільки при зміні стану
    if (digitalRead(ENCODER_DT) != stateCLK) {
      if (freq < FREQ_MAX) {
        freq += 0.1;
        setFrequency(freq);
      } else {
        blinkLED4();  // Блимання при досягненні межі
      }
    } else {
      if (freq > FREQ_MIN) {
        freq -= 0.1;
        setFrequency(freq);
      } else {
        blinkLED3();  // Блимання при досягненні межі
      }
    }
  }
  lastStateCLK = stateCLK;
}

// *** Миготіння LED при досягненні меж ***
void blinkLED4() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}

void blinkLED3() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}

// *** Миготіння LED при скануванні (1 рази) ***
void blinkLED2() {
    digitalWrite(LED, HIGH);
    delay(60);
    digitalWrite(LED, LOW);
    delay(60);
 }

// *** Перевірка рівня сигналу ***
bool checkSignal() {
  Wire.beginTransmission(0x60);
  Wire.write(0x0B);
  Wire.endTransmission();

  Wire.requestFrom(0x60, 2);
  if (Wire.available()) {
    uint8_t rssi = Wire.read() & 0x1F;
    digitalWrite(LED, (rssi > 10) ? HIGH : LOW);
    return rssi > 10;  // Якщо сигнал сильний, повертаємо true
  }
  return false;
}

// *** Авто-скан станцій ***
void autoScan() {
  float scanFreq = freq;

  while (scanFreq <= FREQ_MAX) {
    setFrequency(scanFreq);
    blinkLED2();  // Мигаємо двічі при кожному кроці
    delay(100);   // Затримка для стабілізації

    if (checkSignal()) {  // Якщо знайшли станцію - зупиняємось
      freq = scanFreq;
      scanned = 1;
      return;
    }
    scanFreq += 0.1;  // Переходимо до наступної частоти
  }
}

// *** Налаштування ***
void setup() {
  Wire.begin();
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(SCAN_BTN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  lastStateCLK = digitalRead(ENCODER_CLK);
  setFrequency(freq);
}

// *** Основний цикл ***
void loop() {
  readEncoder();
  
//led if signal rssi>10 good signal *RECOMENDED*
  //checkSignal();
  
//auto scan in start
 // if (digitalRead(SCAN_BTN) == LOW && scanned == 0) {autoScan();}
}
