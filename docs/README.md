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

<div id="bottom"></div>
