# AC Power Current Monitoring - [PZEM-004T-V30](https://github.com/mandulaj/PZEM-004T-v30) & [NODEMCU ESP8266](https://github.com/nodemcu/nodemcu-firmware)
 
AC power current monitoring with web dashboard

## Usage
1. Download or copy **[monit_dashboard.ino](https://github.com/zuna107/AC-power-current-monitoring/blob/26bc68f6e077cffd8c7efb911600d989b5a92949/monit_dashboard/monit_dashboard.ino)** file.
2. Add WiFi credentials.
- WiFi SSID: your WiFi Name.
- Wifi Password: your WiFi Password.

3. Download required libraries:
- **[PZEM-004T-v30](https://github.com/mandulaj/PZEM-004T-v30)**
- **[Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)**
- **[ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer)**

## Schema
![image](https://github.com/zuna107/AC-power-current-monitoring/blob/ed61bd734671982dd76bab30abe929b5737fc22c/circuit_image.png)

## PinOut

**PZEM to NODEMCU**
| PZEM | NODEMCU |
| :--- |:---:    |
| 5V   | 3V3     |
| GND  | GND     |
| TX   | D5      |
| RX   | D6      |

____

**Display SSD1306 to NODEMCU**

| SSD1306 | NODEMCU |
| :---    |:---:    |
| VCC     | 3V3     |
| GND     | GND     |
| SDA     | D2      |
| SCL     | D1      |

____

**Relay Module 5v dual channel to NODEMCU**

| Relay Module | NODEMCU |
| :---         |:---:    |
| VCC          | 5V Input From Adapter or similar | 
|❌ Do not use 3V3 output from NODEMCU |
| GND          | GND     |
| IN 1         | D7      |
| IN 2         | D8      |
