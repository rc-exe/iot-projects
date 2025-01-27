Here’s a step-by-step guide for setting up the weather monitoring system :

---

 Step 1: Install Required Libraries
Ensure the following libraries are installed in your Arduino IDE:
1. LiquidCrystal_I2C: For controlling the I2C LCD.
2. ESP8266WiFi: For connecting the NodeMCU to WiFi.
3. Blynk: For interfacing with the Blynk app.
4. DHT: For working with the DHT11 sensor.
5. SFE_BMP180: For BMP180 pressure sensor readings.

To install these libraries:
- Open Arduino IDE.
- Go to `Sketch` > `Include Library` > `Manage Libraries`.
- Search for the library name and click `Install`.

---

 Step 2: Hardware Setup
1. NodeMCU ESP8266: Acts as the microcontroller.
2. DHT11 Sensor:
   - Connect the `VCC` pin to `3.3V`.
   - Connect the `GND` pin to `GND`.
   - Connect the `DATA` pin to `D3`.
3. BMP180 Sensor:
   - Connect `VCC` to `3.3V`.
   - Connect `GND` to `GND`.
   - Connect `SCL` to `D1`.
   - Connect `SDA` to `D2`.
4. Rain Sensor:
   - Connect the analog output to `A0`.
5. LDR Sensor:
   - Connect the digital output to `D0`.
6. I2C LCD:
   - Connect `VCC` to `3.3V`.
   - Connect `GND` to `GND`.
   - Connect `SCL` to `D1`.
   - Connect `SDA` to `D2`.

---

 Step 3: Configure Blynk App
1. Download the Blynk app from the Play Store or App Store.
2. Create a new project.
3. Add the following widgets:
   - Gauge for Temperature (Virtual Pin V0).
   - Gauge for Humidity (Virtual Pin V1).
   - Gauge for Rain Level (Virtual Pin V2).
   - Gauge for Pressure (Virtual Pin V3).
   - LED for Light Level (Virtual Pin V4).
4. Copy the Blynk authentication token and paste it in the `auth` variable in the code.

---

 Step 4: Update WiFi Credentials
Replace the placeholders in the code with your WiFi details:
```cpp
char ssid[] = "Your_WiFi_Name";
char pass[] = "Your_WiFi_Password";
```

---

 Step 5: Upload the Code
1. Connect the NodeMCU to your computer via USB.
2. Select the correct board and port in the Arduino IDE:
   - Go to `Tools` > `Board` > `NodeMCU 1.0 (ESP-12E Module)`.
   - Go to `Tools` > `Port` > Select the appropriate COM port.
3. Click the upload button to load the code onto the NodeMCU.

---

 Step 6: Test the Setup
1. Power up the NodeMCU.
2. Monitor the serial monitor (set to `9600 baud`) to verify successful WiFi connection and sensor readings.
3. The LCD should display "Weather Monitor System" and then switch between sensor readings and weather conditions.
4. Check the Blynk app for real-time updates.

---

 Step 7: Verify Notifications and Alerts
- Ensure notifications trigger when:
  - Temperature exceeds 35°C.
  - Rain level exceeds 25%.
  - Humidity exceeds 90%.

---

 Troubleshooting
1. If the LCD doesn’t display text, check the I2C address (default is `0x27`) and connections.
2. If WiFi doesn’t connect, verify SSID and password.
3. Use the serial monitor to debug any issues with sensor readings.

