# MPR121 Advanced Calibration
This is for anyone using [Adafruit’s MPR121 Breakout Board](https://www.adafruit.com/product/1982) and, having gone through [Adafruit’s learning guide](https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial) (which BTW is a very good starting point), has reached the limit of what can be done with the Set Thresholds public method of the [MPR121 library](https://github.com/adafruit/Adafruit_MPR121).

This program allows for calibration of individual electrode sensitivity using baseline filtering, touch filtering & debounce, and the MPR121's autoconfig feature, all during program execution, thus avoiding the need to recompile & reload with each scenario being tested. Its objective is finding the optimal calibration parameters to be later implemented in the appropriate production program.

A necessary pre-read to understand this guide is [NXP’s MPR121 Datasheet](https://www.nxp.com/docs/en/data-sheet/MPR121.pdf), particularly *Section 3. Device Operation Overview* and *Section 5. Register Operations Descriptions*. If you don’t know what a register is, go first to  SparkFun’s [Level Up Your Arduino Code: Registers](https://www.youtube.com/watch?v=6q1yEb_ukw8). And if you’re unfamiliar with binary operations, start with their [Digital Logic Tutorial]( https://learn.sparkfun.com/tutorials/digital-logic). 

You may notice in NXP’s website that the venerable MPR121 is no longer manufactured, but there are [plenty pieces in inventory to last us a long time]( https://www.arrow.com/en/products/mpr121qr2/resurgent-semiconductor-llc), so there’s nothing to worry about (unless of course, your plan was to create a ground-breaking product to bring riches and glory, in which case, you should consider some other cap sensor).

If you find that I’ve missed on something, then by all means enrich this experience with your dialogue, which can only result in benefits for us all.

### Cap Sensor Sensitivity ###
The MPR121 can measure capacitances ranging from 10 pF to 2000 pF (approx.) by varying the amount of charge current and charge time applied to the sensing inputs. This means that it may not be enough to go through certain materials. (See [AN3863 Designing Touch Sensing Electrodes](https://www.nxp.com/docs/en/application-note/AN3863.pdf)). In any case, charge current and charge time can be modified to reach other capacitance sensitivities.

At the end of the charge time, the electrode voltage is measured with a 10 bit ADC. The highest operating range of the electrode charging source is Vdd - 0.7V, so, it is critical not to charge the electrode beyond this point because it’s a non-linear region, meaning the ADC reading can quickly overflow returning meaningless results. For more information see [AN3889 Capacitance Sensing Settings](https://www.nxp.com/docs/en/application-note/AN3889.pdf).

This is why the Autoconfig feature is so sweet: it automatically tests various charge times and charge currents to determine the maximum values (within definable limits) that result in controlled output, and it does this *for each individual electrode*, allowing for differences in capacitance due to conector length, pad characteristics, etc.  Details on the input variables for Autoconfig start in Page 7 of [AN3889](https://www.nxp.com/docs/en/application-note/AN3889.pdf).

### MPR121 Library ###
To use Autoconfig and to change calibration parameters during program execution, some serious modifications to the Adafruit_MPR121 library files are necessary, so I copied and renamed the .cpp and .h files and placed them in the same file directory of the .ino sketch. This way the original MPR121 library remains untouched.

The changes to the library include:
* Run-time defined parameters for:
  * Baseline Filtering Control
  * Touch/Release Debounce
  * First & Secondary Filtering and Sample Interval
  * Autoconfig Level Limits & Target
* Binary encoding of Filter and Global CDC CDT Configuration registers
* Display of resulting Charge Time and Charge Current per electrode defined by Autoconfig
* Differentiating failure reason when starting the board
* Clean-up of unused code

### Main Program (.ino file) ###
My use case has three breakout boards with specific I2C addresses, but this can be changed in the variable definition section of the main sketch.

Instead of using the standard MPR121 library methods to acquire status data (touched, filereredData & baselineData), the program executes a multibyte read of all status registers (from 0x00 thru 0x2A) at once, providing a coherent picture (same time stamp) for analysis of touched/released status and its associated filtered and baseline data.

Other options provided by the program for output data (to plot or print) include:
* Filtered data of a selected board or all of them
* Baseline data of a selected board or all of them
* Baseline & Filtered data of a selected board

### Blynk as Input Data Interface ###
Although designed as a platform for IoT software, a free account (two devices max) can be used to create, fast & easy, a user interface to input data. It's well [documented](https://docs.blynk.io/en/) and has a [Quickstart Guide](https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk) that will get you running in minutes. I'm using an [ESP32 Feather V2](https://www.adafruit.com/product/5400) and there is a long list of [supported hardware](https://github.com/blynkkk/blynkkk.github.io/blob/master/SupportedHardware.md). For ESP32's, use the [BlynkSimpleEsp32](https://github.com/blynkkk/blynk-library/blob/888a81dd84ec8bab19d8652654d03c58aa12646b/src/BlynkSimpleEsp32.h) library that can be installed via Arduino IDE following [these instructions](https://docs.blynk.io/en/blynk.edgent-firmware-api/installation/what-do-i-need-to-blynk).

Below is a screenshot of what the Web Dasbhboard can look like: adjust any MPR121 parameter and then click on START to issue the cap.begin() method and restart the breakout boards with the new parameters.

![Screenshot 2022-07-16 182932](https://user-images.githubusercontent.com/33431200/179375026-d2b4b2d4-7a97-4b32-a04d-ed7c14d7b3f9.jpg)


### Other Useful Application Notes ###
Finally, for further understanding of what can be done with the MPR121 cap sensor, consider all the [application notes available](https://www.nxp.com/products/no-longer-manufactured/proximity-capacitive-touch-sensor-controller:MPR121), particularly:
* AN3890 [Filtering & Timing](https://www.nxp.com/docs/en/application-note/AN3890.pdf)
* AN3891 [Baseline System](https://www.nxp.com/docs/en/application-note/AN3891.pdf)
* AN3892 [Jitter and False Touch](https://www.nxp.com/docs/en/application-note/AN3892.pdf)


