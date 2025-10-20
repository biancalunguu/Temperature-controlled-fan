#include <DHT.h>
#include <LiquidCrystal.h>

#define DHT_SENSOR_PIN 2
#define DHT_SENSOR_TYPE DHT11
#define BUTTON_PIN 7
#define ENABLE_PIN 5
#define DIRA_PIN 3
#define DIRB_PIN 4

const float TEMPERATURE_THRESHOLD = 25.0;

DHT dht(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

bool fan_on = false;

const long debounceDelay = 50;
long lastDebounceTime = 0;
int lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  Serial.println("Fan Control System Initializing...");

  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(DIRA_PIN, OUTPUT);
  pinMode(DIRB_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("Fan Control");
  lcd.setCursor(0, 1);
  lcd.print("Init...");

  delay(1000);
  
  dht.begin();
  lcd.clear();
}

void loop() {
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(temperature) || isnan(humidity)) {
    lcd.clear();
    lcd.print("Error reading DHT");
    Serial.println("Error reading DHT sensor.");
    return;
  }
  
  int currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentButtonState == LOW) { 
      fan_on = false; 
      
      digitalWrite(ENABLE_PIN, LOW); 
      digitalWrite(DIRA_PIN, LOW);
      digitalWrite(DIRB_PIN, LOW);
      
      Serial.println("Button pressed! Fan reset to OFF.");
      
      delay(500); 
    }
  }

  lastButtonState = currentButtonState;

  if (temperature > TEMPERATURE_THRESHOLD && !fan_on) {
    digitalWrite(ENABLE_PIN, HIGH);
    fan_on = true;
    
    digitalWrite(DIRA_PIN, HIGH);
    digitalWrite(DIRB_PIN, LOW);

    Serial.print("Fan ON: Temperature > ");
    Serial.print(TEMPERATURE_THRESHOLD);
    Serial.println("C");

  } else if (temperature <= TEMPERATURE_THRESHOLD && fan_on) {
    digitalWrite(ENABLE_PIN, LOW);
    fan_on = false;

    digitalWrite(DIRA_PIN, LOW);
    digitalWrite(DIRB_PIN, LOW);
    
    Serial.print("Fan OFF: Temperature <= ");
    Serial.print(TEMPERATURE_THRESHOLD);
    Serial.println("C");
  }

  
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  if (fan_on) {
    lcd.print("FAN: ON       ");
  } else {
    lcd.print("FAN: OFF      ");
  }

  delay(2000); 
}
