# Smart Home Appliance (Lamp+)

This project is about creating a multi-purpose Smart Lamp, that's meant to sit on small cabinet next to your bed.


###### This Smart Home Appliance project is partly a project task for AGH Embedded Systems Course (pl.: "Systemy Wbudowane").

##### Work logs: [link](https://github.com/GabenRulez/SmartHomeAppliance/blob/main/docs/README.md) 
___

### Functions & premises:

 - It lights up not only with warm-white LEDs, but also RGB ones. 
 - It can be controlled either through rotary encoder and OLED display on the front of the device, or from HTTP requests sent through WiFi.
 - It has multiple lighting modes, some with addressable mechanic.
 - Data from light sensor will be used to brighten/darken/turn off the lamp.
 - Easily powered through 5V DC by USB-C port (every phone charger).
 - ###### Maybe: Bluetooth speaker + LEDs controlled by playing music.

### Used parts:
 - ESP32 board ([ESP32-DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html))
![](/media/photos/ESP32_01.jpg)
 - 5V addressable RGB LED strips (WS2813)
![](/media/photos/LEDstripA_01.jpg)
 - 12V warm white LED strips
![](/media/photos/LEDstripB_01.jpg)
 - 0.91 inch I^2C white OLED module with a resolution of 128x32
![](/media/photos/OLED_01.jpg)
 - Light sensor module with LM393 and photoresistor
![](/media/photos/LightSensor_01.jpg)
 - EC11 5-pin Rotary encoder
![](/media/photos/RotaryEncoder_01.jpg)
 - MT3608 Step-Up-Boost-Converter
![](/media/photos/StepUp_01.jpg)
 - Simple switch
![](/media/photos/Switch_01.jpg)
 - 6-pin USB-C port
![](/media/photos/USB-C_01.jpg)
 - some capacitors
![](/media/photos/CapacitorA_01.jpg)
 - IRF 840 N-MOSFET
  ![](/media/photos/NMosfet.jpg)

___

### Drivers

To make my ESP32 be recognisable by my Windows machine I installed [CP210x Universal Windows Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) from [Silicon Labs](https://www.silabs.com/).

Copy of exact version I installed is [here](/extra/driver).

___

### Workspace

I used [Arduino IDE](https://www.arduino.cc/en/software) to develop this application.

To make ESP32 work with it, I had to include in `File` > `Preferences` > `Settings` > `Additional Boards Manager URLs:` line: `https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json` (if this space is not empty, then simply put those after `,` (comma)).

Then I went into `Tools` > `Board` > `Boards Manager`, searched for `ESP32` and installed the addon by "Espressif Systems".

Then from `Tools` > `Board` > `ESP32` I chose `DOIT ESP32 DEVKIT V1` (sadly there isn't any listed `DEVKIT V4` board).

___

### Libraries

###### All those libraries are backup'ed [here](https://github.com/GabenRulez/SmartHomeAppliance/tree/main/code/SmartHomeAppliance/libraries).

- [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) - OLED display library
- [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) - graphics library used by `Adafruit_SSD1306`
- [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) - library to take care of `I^2C` communication
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - library to create asynchronous web server
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) - dependable for `ESPAsyncWebServer`

