### Working logs
<a href="#bottom">Go to newest message</a>
###### I have just started this log file (2021-04-13) so earlier dates might be slightly incorrect.

___

#### 2021 04 11

As the screen was already working correctly, I decided to make next step and connect the rotary encoder.
I used jumper cables. I placed the encoder quite far from the board itself (near the display, at the top of the breadboards).

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

<div id="bottom"></div>
