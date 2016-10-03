const int buttonPin = 11;    // Кнопка спуска
const int ledPin = 12;       // Пин с сигнельным светодиодом
const int triggerPin = 10;   // MOSFET с реле
const int buzzerPin = 9;     // Пищалка
const int analogPin = A3;    // Переменный резистор 10К для выставления длины импульса

// Объявляем переменные:
int WeldingNow = LOW;
int buttonState;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;    // минимальное время в мс, которое надо выждать до срабатывания. Сделано для предотвращения ложных срабатываний при дребезге контактов спусковой кнопки

int sensorValue = 0;        // считываем значение, выставленное на потенциометре в эту переменную...
int weldingTime = 0;        // ...и на его основе выставляем задержку

void setup() {
  pinMode(analogPin,  INPUT);
  pinMode(buttonPin,  INPUT);
  pinMode(ledPin,     OUTPUT);
  pinMode(triggerPin, OUTPUT);
  pinMode(buzzerPin,  OUTPUT);
  digitalWrite(ledPin,     LOW);
  digitalWrite(triggerPin, LOW);
  digitalWrite(buzzerPin,  LOW);

  Serial.begin(9600);
}

void loop() {

  sensorValue = analogRead(analogPin); // считываем значение, выставленное на потенциометре
  weldingTime = map(sensorValue, 0, 1023, 15, 255); // приводим его к миллисекундам в диапазоне от 15 до 255
  Serial.print("Analog pot reads = ");
  Serial.print(sensorValue);
  Serial.print("\t so we will weld for = ");
  Serial.print(weldingTime);
  Serial.println("ms. ");

  // Для предотврещения ложных срабатываний кнопки убеждаемся сначала, что она зажата минимум в течение 50мс, прежде чем начать сварку:
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        WeldingNow = !WeldingNow;
      }
    }
  }

  // Если команда получена, то начинаем:
  if (WeldingNow == HIGH) {

    Serial.println("== Welding starts now! ==");
    delay(1000);

    // Выдаём три коротких и один длинный писк в динамик:
    int cnt = 1;
    while (cnt <= 3) {
      playTone(1915, 150); // 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956
      delay(500);
      cnt++;
    }
    playTone(956,  300);
    delay(1);

    // И сразу после последнего писка приоткрываем MOSFET на нужное количество миллисекунд:
    digitalWrite(triggerPin, HIGH);
    delay(weldingTime);
    digitalWrite(triggerPin, LOW);

    Serial.println("== Welding ended! ==");
    delay(1000);

    // И всё по-новой:
    WeldingNow = LOW;

  } else {
    digitalWrite(ledPin,     LOW);
    digitalWrite(triggerPin, LOW);
    digitalWrite(buzzerPin,  LOW);
  }

  lastButtonState = reading;
}

// В эту функцию вынесен код, обслуживающий пищалку:
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(tone);
  }
}
