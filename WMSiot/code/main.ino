#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <SFE_BMP180.h>

// Initialize LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// object for the BMP180 sensor
SFE_BMP180 bmp;

char auth[] = "rnwdBnNLQfxwfSUBwDlsmXIQHb_ce9De"; // Auth token
char ssid[] = "ritesh"; //  WIFI name
char pass[] = "ritesh1234"; //  WIFI password

DHT dht(D3, DHT11); // (DHT sensor pin, sensor type)
BlynkTimer timer;

// Define Rain and LDR pins
#define rain A0
#define light D0

// Create variables for pressure
double T, P;
char status;

// Flags for warnings
bool tempWarning = false;
bool humidityWarning = false;
bool rainWarning = false;

// Time management
unsigned long previousMillis = 0;
const unsigned long displayInterval = 2000; // Interval for switching between warnings and readings
unsigned long previousPressureMillis = 0;
const unsigned long pressureInterval = 2000; // Interval for pressure display
bool displayingReadings = true;

void setup() {
  Serial.begin(9600);
  bmp.begin();
  lcd.begin(D2, D1); // Initialize LCD with parameters
  lcd.backlight();
  lcd.print("connecting..");
  pinMode(light, INPUT);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  lcd.clear();
  lcd.print("connected");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weather Monitor");
  lcd.setCursor(4, 1);
  lcd.print("System");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading Sensors");
  lcd.setCursor(0, 1);
  lcd.print("please wait");
  delay(4000);

  // Call the functions
  timer.setInterval(1000L, DHT11sensor); // timing
  timer.setInterval(1000L, rainSensor);
  timer.setInterval(1000L, LDRsensor);
}

void displayReadings() {
  lcd.clear();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

  lcd.setCursor(0, 1);
  int rainValue = analogRead(rain);


  if (rainValue < 10) {
    rainValue = 0;
  } else {
    rainValue = map(rainValue, 0, 1024, 100, 0);
  }

  lcd.print("R:");
  lcd.print(rainValue);

 
  lcd.setCursor(8, 1);
  lcd.print("P:");
  lcd.print((int)P);
}

void displayWeatherCondition() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weather:");
int ldrValue = digitalRead(light); // LDR value

 
  if (ldrValue == LOW) { 
    lcd.setCursor(9, 0);
    lcd.print("Day");
  } else {
    lcd.setCursor(9, 0);
    lcd.print("Night");
  }

  lcd.setCursor(0, 1);
  lcd.print(getWeatherCondition());
}
 

String getWeatherCondition() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int rainValue = analogRead(rain);
  bool lightLevel = digitalRead(light);
  bmp.startTemperature();
  bmp.getTemperature(T);  
  bmp.startPressure(3);
  bmp.getPressure(P, T);  

  
  rainValue = (rainValue < 10) ? 0 : map(rainValue, 0, 1024, 100, 0);
  
  
  if (h > 70) {
    return "Likely Rain/Fog";
  } else if (h < 40) {
    if (rainValue > 25) {
      return "Heavy Rain";
    } else if (rainValue > 10 && rainValue <= 25) {
      return "Light Rain/Drizzle";
    } else {
      return "Dry & Clear";
    }
  }

  if (t > 30) {
    return "Hot Weather";
  } else if (t < 25) {
    if (h > 70) {
      return "Cool & Humid";
    } else {
      return "Cool & Clear";
    }
  }

  if (P > 1000) {
    return "Fair Weather";
  } else if (P > 1020) {
    return "Possible Storm";
  }

  if (rainValue > 25) {
    return "Heavy Rain";
  } else if (rainValue > 5 && rainValue <= 25) {
    return "Light Rain";
  } else {
    return "Cloudy";
  }
}

void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send temperature to Blynk
  Blynk.virtualWrite(V0, t);  

  // Send humidity to Blynk
  Blynk.virtualWrite(V1, h);

  
  if (t > 35.0) { // Example threshold
    tempWarning = true;
    Serial.println("Temperature is too high! Logging event...");
    Blynk.logEvent("temp_high", "Temperature is too high!");
  } else {
    tempWarning = false;
  }

  
  if (h > 90.0) { // Example threshold
    humidityWarning = true;
  } else {
    humidityWarning = false;
  }

  
  Serial.print("Temperature: ");
  Serial.println(t);
}

void rainSensor() {
  if (!tempWarning && !humidityWarning) {
    int value = analogRead(rain);

    
    if (value < 10) {
      value = 0;
    } else {
      value = map(value, 0, 1024, 100, 0);
    }

    Blynk.virtualWrite(V2, value);

    if (value > 25) { 
      rainWarning = true;
      Serial.println("Rain level is too high! Logging event...");
      Blynk.logEvent("rain_high", "Rain level is too high!");
    } else {
      rainWarning = false;
    }
  }
}

void pressure() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousPressureMillis >= pressureInterval) {
    previousPressureMillis = currentMillis;

    if (!tempWarning && !humidityWarning && !rainWarning) {
      status = bmp.startTemperature();
      if (status != 0) {
        delay(status);
        status = bmp.getTemperature(T);

        status = bmp.startPressure(3); // 0 to 3
        if (status != 0) {
          delay(status);
          status = bmp.getPressure(P, T);
          if (status != 0) {
            Blynk.virtualWrite(V3, P);
          } else {
            Serial.println("Failed to get pressure");
            P = 0; 
          }
        } else {
          Serial.println("Failed to start pressure measurement");
          P = 0; 
        }
      } else {
        Serial.println("Failed to start temperature measurement");
        P = 0; 
      }
    }
  }
}

void LDRsensor() {
  if (!tempWarning && !humidityWarning && !rainWarning) {
    bool value = digitalRead(light);
    if (value == 0) {
      WidgetLED LED(V4);
      LED.on();
    } else {
      WidgetLED LED(V4);
      LED.off();
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= displayInterval) {
    previousMillis = currentMillis;

    if (displayingReadings) {
      displayReadings();
      displayingReadings = false; 
    } else {
      displayWeatherCondition();
      displayingReadings = true; 
    }
  }

  pressure(); // Update pressure data
  Blynk.run(); // Blynk library
  timer.run(); // Blynk timer
}
