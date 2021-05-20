### Working logs
<a href="#bottom">Go to newest message</a>
###### I have just started this log file (2021-04-13) so earlier dates might be slightly incorrect.

___

#### 2021 03 25

I decided on the parts that I'm gonna use in this project. I documented them in the `README.md` in the main folder of this repository.

___

#### 2021 04 08

I downloaded libraries, that I plan on using with this project.
I created a simple sketch with Adafruit library to test the display.

___

#### 2021 04 11

As the screen was already working correctly, I decided to make next step and connect the rotary encoder.
I used jumper cables. I placed the encoder quite far from the board itself (near the display, at the top of the breadboards).

![](/media/photos/workLog/CircuitState_02.jpg)

I turned the appropriate pins in Arduino IDE, compiled and uploaded the sketch.

I encountered a weird "bug"(?) - when my hand was close (but not touching) the cables, the whole ESP32 crashed repeatedly.
Cables were all very close to each other, so I placed them further apart, yet the crashing didn't stop.

___

#### 2021 04 12

Hours of debugging (rotary encoder):

I connected pins `A` and `B` to oscilloscope and made few observations there:
 - Signals `A` and `B` were just some noise. => I've put resistors in between ground and `A` and `B` outputs to discharge this noise.
 - Signals `A` and `B` had characteristics of charging/discharging capacitor (some parasitic capacity). => The resistors I added earlier had too much resistance (330kΩ), so I replaced them with pull-up resistors on ESP32.
 - Signals finally seem fine, but the interrupts set as `attachInterrupt(ROTARY_ENCODER_PIN_A, rotaryEncoderInterrupt, RISING);` were triggered not once, but about 50 to 250 times per single "click" (1/20th of the rotation). => I've added a low-pass filter to those inputs.
 - Interrupts were triggered less often, but still about 5 to 20 times per "click". => With further assistance of my friend's oscilloscope I found out, that there were random "pin vibrations"(Sorry, I don't know the English word for "drgania styków"). 
   ESP interrupts triggered on signals just 10ns long!

To somehow minimize those "random" I decided to code in "debounce" functionality, as I've often used it while programming in JavaScript.

It turned out, that it's harder than it looks in embedded system.
I had to utilize FreeRTOS tasks, as Arduino itself doesn't have any schedulers.

1. I connected interrupt to pin `A` on `RISING`. 
2. Immediately after interrupt is triggered, the state of pin `B` is read (`digitalRead`). 
3. Then the interrupt function calls `xTimerPendFunctionCallFromISR` and passes `pinBState`.
4. That called function now checks, whether the selected task handler is free.
   - If it's taken, then it stops that task using `vTaskDelete`
   - Now it's in state, where task handler is free, so it starts the new "debounced" task
5. The "debounced" tasks waits for selected period of time (later set at 33 miliseconds) using `vTaskDelay`.
6. Then it enters critical section (`taskENTER_CRITICAL(mutex)`) so that it's not interrupted while working, updates `rotaryEncoderValue` accordingly and exits the critical section.  

After some testing and calibrating the `debounceTime`, the rotary encoder worked much better.

P.S I also replaced the jumper cables with some thicker gauge wire. It's much cleaner now.

![](/media/photos/workLog/CircuitState_03.jpg)

___

#### 2021 04 13

I decided to put the display functions inside it's own task.
I created `screenManagerTask`, which `FPS` times per second: created copy of `rotaryEncoderValue`, cleared display and showed the text.

After uploading it created very weird graphical glitches.
After some reading, I came to conclusion that it was due to `I^2C` protocol being used on different cores.

So I used `xTaskCreatePinnedToCore` and run this task on the same core, that the `setup()` function was run on.
This fixed the weird glitch.


I was still unhappy about the weird "twitching" of the value from the encoder. From the Distributed Systems course, I knew, that multithreading access should be controlled (e.g. with mutexes).
At first I used `xSemaphoreCreateMutex`, but then found out, that it's not recommended for multicore applications.
I switched to `xSemaphoreCreateBinary` instead.

Later I decided to put all the rotary encoder code into one proprietary `C++` class. It took me some time, but then it turned out, that the functions I used with interrupts have to operate on the public variables and static functions, so I couldn't enclose it completely. 
As I didn't want to split the code into "some into the class, some not" I foregone this idea.

___

#### 2021 04 14

I wanted to finally attach white 12V LEDs to the board.

I took out the step-up boost converter, inserted gold-pins, put it in separate breadboard and soldered the pins. Then I connected the boost converter to 5V DC phone charger and measured the voltage across the `VOUT+` and `VOUT-` pins.

The voltage behaved very weirdly. It started at 5V. Then after ~5 seconds it went up to 10V, so I calibrated it to 12V. Then after ~30 seconds the voltage started going up. It went up and up and up. The biggest voltage I saw was 38V (According to the circuit documentation that's not even possible)!

I had a thought, that maybe it's because there is no connected load, so some capacitor isn't discharging. 

I connected a small strip of white 12V LEDs to the output and measured the voltage. They turned on after ~5 seconds, then they were sitting at ~11V for about 2 minutes. Then suddenly the voltage spiked to 23V, the LEDs started smoking, so I immediately turned them off, but I couldn't turn them on any longer.

![](/media/photos/workLog/smokedLEDs.jpg)

The culprit:

![](/media/photos/workLog/brokenBoostConverter.jpg)

I then went ahead and repeated the whole process (luckily I had 5 of them ordered at the same time). The new one worked correctly :)

![](/media/photos/workLog/newBoostConverterWorking.jpg)

___

#### 2021 04 15

I somehow forgot about the button inside the rotary encoder, thus I decided to come back to it and make it work.

![](/media/photos/workLog/CircuitState_04.jpg)

I created a simple function and attached it as an interrupt (`RISING`) to the button pin. This function simply outputed "ButtonPressed" on the serial monitor.

When I pushed down the button the interrupt triggered once (as it should) or sometimes twice, but when I let go off the button then the interrupt triggered some more times.

My "Embedded Systems" course professor proposed to change the ESP32 internal pull-up resistors for some external ones, of lower resistance (~30kΩ -> 1kΩ). This resulted in much better looking wave (I connected the outputs to the oscilloscope earlier) and the triggered interrupts followed.

Now I was at a point, where:

- pressing down triggered once, and only once
- pressing up (letting go) didn't triggered, or triggered just once

I decided to try out something different. ESP32 is very powerful chipset, so I don't really have to mind using more CPU intensive ways.

I creating polling function, instead of an interrupt-based one. To get rid of this small little `down-up-down` signal when letting go of the button, my function:

- reads the digital value of button pin
  - if it's state is `HIGH` :
    - waits for few miliseconds
    - reads the digital value again
  - if it's state is `LOW`:
    - does nothing and just waits for the next loop

Doing it like this eliminates any possibility of wrongly reading few nanoseconds long `HIGH` pulses. It creates some load on the CPU, but as I said - it's not as much, as to make it matter here.

So now I have a variable, which states in which state is the button, that is updated `FPS` times per second (it's set up as 60 for now; I'll create it own variable later). If value changes from `0` to `1`, then I know the button was pressed.

___

#### 2021 04 17

I tried to change the way I use ESP-IDF. Till now I was using Arduino IDE with ESP32 extension, but there's a way to work with just ESP-IDF.

I installed Visual Studio Code, installed PlatformIO and then tried to create a new project, but I couldn't.

ESP-IDF forbids any spaces (" ") in the path of your OS installation. Unfortunately, mine has one, and it's impossible to change it, unless I reinstall Windows 10. I can't really do that right now, so I have to forfeit that idea.

Instead I reformatted my code in ArduinoIDE, created additional `config.h` and `secretConfig.h` files, as well as some `.ino` files - each for some part of the project, e.g. `rotaryEncoder.ino` and `OLEDScreen.ino`.

___

#### 2021 04 21

I downloaded few libraries: `ESPAsyncWebServer` and `AsyncTCP`, that allow to create asynchronous web server. 

I also imported `<WiFi.h>` to the ArduinoIDE project.

I decided upon the WiFi working mode - `Station + AP`. It basically allows the ESP32 to be connected to the web, as well as create its own local network (Access Point), through which you can change the lamp options. 

During testing I found out, that I can't really connect it as `Station`  to the hotspot that is created by my laptop. I would need a proper router for that. So for now I will omit the `Station` connection and just work on `AP` part.

___

#### 2021 04 25

I decided to hook up the 12V LEDs (as they are supposed to be a trickier part -> they require `12V` which is non-existant at the moment and they're supposed to be controlled through PWM signal from ESP32).

I hooked up my MOSFET according to it's documentation, added some load in the circuit and tried to test everything.

I encountered some hiccups, where e.g. the `GATE` pin required resistor in line (I was taught, that there is no current through gate and that would mean, that no resistor is required). Also - the ESP32 `5V` port turned out to be too weak (I knew it would happen, but here the current should be pretty limited. I placed `10kΩ` load, so one would think, that the maximum current would be around `1.2mA` ). Due to `High Power Draw` from my laptop's `USB` terminal, Windows 10 blocked my `USB` terminals, thus requiring me to reset those ports + restart PC.

I disconnected `5V` rail from ESP32 and connected it to a phone charger port. That way, this `5V` rail can support over `2A` of current.

Apart from that I somehow fried the step-up-converter. I had enough of the problems, so I stopped work for the day.

___

#### 2021 04 28

I finally got myself a proper router. I can now work on `Station + AP` mode on ESP32.

I started by cleaning up all the code regarding asynchronous web server. I also created few routers, that will allow frontend to maintain connection with the server.

Added `startStation` function, that tries to connect and checks `RECONNECTION_MAX_COUNT` times (every `RECONNECTION_TIMEOUT` milliseconds) whether the ESP successfully connected to the SSID. If not -> it stops checking.

I started creating a HTML website, that will work as user interface through web connection.

___

#### 2021 04 29

I implemented "modes" functionality. The screen now checks, whether the mode changed from last frame, and then displays accordingly.

Asynchronous server also listens on `/warmLights` and `/modeOFF` requests to change those modes.

I also added buttons, that send these requests to the ESP32.

Apart from that I designed and created a UI for the web. 

![](/media/photos/workLog/webUI_1.png)

There is an indicator in the top-left corner, which every 5 seconds sends a request `/alive` to the server. If the server doesn't respond in 2 seconds, the indicator will turn red, if it responds - green.

As the design is fully responsive, the selecting cards take most of the screen estate, however we see a bit of neighbouring cards on left and right. There is also a menu on top, which corresponds to the selected cards.

___

#### 2021 05 08

Shops finally opened and I was able to go and buy some "diffuser" for the LEDs.

I decided upon buying cheap ~1$ plastic, white Ikea plant holder "[Nejkon](https://www.ikea.com/pl/pl/p/nejkon-oslonka-doniczki-bialy-80285792/)". It turned out alright, as it allowed some of the light through it. 

![](/media/photos/workLog/LEDInAJar.gif)

If it turns out to be absorbing too much light, then I might just buy the cheapest lamp there is, break it into parts and use it's diffuser instead.

___

#### 2021 05 09

I took all the `programModes` variables that were "somewhere" and put them into one, solid `struct`. That way, the screen can have its own local copy and update it, as a whole at once, thus reducing the amount it takes the semaphore.

I updated the HTML page to also send range slider value, as `strength=` parameter. Buttons also change their form, when they haven't yet received the appropriate response from the server.

Then I updated the server query, to collect this `strength=` parameter and then display it on the OLED screen.



Later I created an `RGBLights` router, that takes in `red`, `green` and `blue` parameters, then display on the OLED screen. ![](/media/photos/workLog/rgbOnDisplay.jpeg)

 I updated the HTML page, to allow it to select any RGB value. I used [external library called "Reinvented Color Wheel"](https://github.com/luncheon/reinvented-color-wheel) I found on the internet. Overall the page came out nicely (at least in my opinion :smiley:).

![](/media/photos/workLog/webUI_2.png)

___

#### 2021 05 11

I hooked up the addressable RGB strip to the 5V rail and connected its data pin to `GPIO 32`.

Then I installed the [Adafruit  NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel) and tried few template configurations. I encountered some problems, such as that the LEDs were constantly changing their brightness and colors, as it was selected by random, while my functions should display a "rainbow" pattern.

Turns out it was due to different grounds (`5V` rail was disconnected from the ESP32 and connected into `5V` source, USB charger). After powering the whole setup through just `5V` source, the problem was no more.

Due to constantly changing and stuttering light source I got sick, so I just quickly wrote some functions to change color, according to HTTP request. It all worked out, but I plan on creating a worker task, that would receive commands, such as "turn LEDs green", or "play rainbow animation".

![](/media/photos/workLog/RGBLEDInAJar.gif)

Oh, and I made the transition smooth, by linearly interpolating between the two values.

___

#### 2021 05 13

I asked my college professor for tips regarding problems with PWM. We refactored how the circuit should look and decided upon first checking, whether the MOSFET and Boost-Up-Converter are working correctly.

![](/media/photos/workLog/schematic_1.png)

Before I wired it as in the schematic I found one source of an earlier problem - when I connect LED strip to `12V` source, the `5V` rail voltage falls to just `3.7V` - and that is the reason why ESP32 was constantly crashing.

![](/media/photos/workLog/CircuitState_05_pre.jpg)

![](/media/photos/workLog/CircuitState_05_post.jpg)

 I cut off most of the LED strip and left just 40cm = 24 LEDs.

Then I wired everything as in the schematic. It was working now with no crashes and no smoke :smile:

![](/media/photos/workLog/WarmLightsMOSFET.gif)

___

#### 2021 05 14

I created a separate task `LEDControllerTask`, which directly controls the LED strips. It receives `LEDControllerCommands` through FreeRTOS `queue`s.

I got rid of any time-demanding things in handling server requests. Now the handlers just verify whether the request is correct, and then sends commands to LED controller.

Now it's properly handling:

- static RGB color
- smooth transitions between static colors, turning ON / OFF
- two color mode (sinusoidally changing between those two)

Added API call for the new `twoColors` mode. I will need to add configurator to GUI. It works though, with commands like 
`192.168.0.102/twoColors?red1=80&green1=200&blue1=0&red2=200&green2=0&blue2=200`

Due to some bugs that were happening when `worker` was being shut down while using `LEDStrip.show()` I added semaphore, so that the task will enter a critical section while doing this.



Later in the day I went to IKEA again, as I decided to buy some cheap lamp instead of this "Nejkon" plant holder. I bought "[Lampan](https://www.ikea.com/pl/pl/p/lampan-lampa-stolowa-bialy-20046988/)" for about ~5$.

![](/media/photos/workLog/Lampan_1.jpg)

I took the power cord apart.

![](/media/photos/workLog/Lampan_parts.jpg)

Overall it looks much nicer, as it lets more light through. Also this base is also lighting up with LEDs, further dispersing the light.



I decided to finally code in the `PWM` functionality to the program. Fortunately it went relatively easy now (nothing burned down) and it was up and running quickly.

![](/media/photos/workLog/CircuitState_06.jpg)

I cleaned up the whole circuit with thicker cables.

![](/media/photos/workLog/CircuitState_07.jpg)

I went on and cleaned up all the `PWM` code. I rewrote the server route, added proper handling of the command to `LEDControllerTask` and coded smooth transition for `ON` /`OFF`.

I tested everything and it works as intended.

___

#### 2021 05 20

I added the newly created `twoColors` mode to the GUI.

Then I went ahead and implemented `rainbow` mode (each RGB LED is in different color).

Everything went smoothly.

___

<div id="bottom"></div>