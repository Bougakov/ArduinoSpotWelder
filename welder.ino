// BOM:

// Relay: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2F2016-High-Quality-12V-Starter-Relay-Solenoid-Cable-for-Quad-Pit-Bike-110cc-125cc-Safety-Driving%2F32681191278.html 
// Metal strip: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2F1-PC-1M-8mm-x-0-15-Pure-Ni-Plate-Nickel-Strip-Tape-For-Li-18650%2F32617548208.html 
// Nano: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2FWith-the-bootloader-Nano-3-0-controller-compatible-for-arduino-nano-CH340-USB-driver-NO-CABLE%2F32341832857.html 
// Pushbutton: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2F10-Pcs-lot-12mm-x-12mm-x-7-5mm-Panel-PCB-Momentary-Tactile-Tact-Push-Button%2F32219609083.html 
// Opto: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2F50034-Free-shipping-20pcs-High-Density-Mounting-Type-Photocoupler-DIP-4-PC817C-DIP-Optocoupler-PC817%2F32342458824.html 
// XM1584: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2FFree-Shipping-5PCS-XM1584-Ultra-small-size-DC-DC-step-down-power-supply-module-3A-adjustable%2F32262690691.html 
// LED: https://alitems.com/g/1e8d1144949a590a4ec116525dc3e8/?ulp=https%3A%2F%2Fwww.aliexpress.com%2Fitem%2F100Pcs-5mm-White-Ultra-Bright-LED-Light-Lamp-Emitting-Diodes-15000MCD%2F1831428516.html

const int buttonPin = 11;    // Кнопка спуска
const int ledPin = 12;       // Пин с сигнальным светодиодом
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
    digitalWrite(ledPin,     HIGH);
    digitalWrite(triggerPin, HIGH);
    delay(weldingTime);
    digitalWrite(triggerPin, LOW);
    digitalWrite(ledPin,     LOW);

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
  digitalWrite(ledPin, HIGH);
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(tone);
  }
  digitalWrite(ledPin, LOW);
}
