# MPR121 Advanced Calibration
This is for anyone using [Adafruit’s MPR121 Breakout Board](https://www.adafruit.com/product/1982) and, having gone through [Adafruit’s learning guide](https://learn.adafruit.com/adafruit-mpr121-12-key-capacitive-touch-sensor-breakout-tutorial) (which BTW is a very good starting point), has reached the limit of what can be done with the Set Thresholds public method of the [MPR121 library](https://github.com/adafruit/Adafruit_MPR121).

These programs allow for calibration of individual electrode sensitivity using baseline filtering, touch filtering & debounce, and, the MPR121's autoconfig feature, all during program execution so as to avoid the need to recompile & reload with each scenario being tested.

A necessary pre-read to understand this guide is [NXP’s MPR121 Datasheet](https://www.nxp.com/docs/en/data-sheet/MPR121.pdf), particularly *Section 3. Device Operation Overview* and *Section 5. Register Operations Descriptions*. If you don’t know what a register is, go first to  [SparkFun’s Level Up Your Arduino Code: Registers](https://www.youtube.com/watch?v=6q1yEb_ukw8). And if you’re unfamiliar with binary operations, start with their [Digital Logic Tutorial]( https://learn.sparkfun.com/tutorials/digital-logic). 

You may notice in NXP’s website that the venerable MPR121 is no longer manufactured, but, because there are [plenty pieces in inventory to last us a long time]( https://www.arrow.com/en/products/mpr121qr2/resurgent-semiconductor-llc), there’s nothing to worry about (unless, your plan was to create a ground-breaking product to bring riches and glory, in which case, you should consider some other cap sensor).

If you find that I’ve missed on something, then by all means enrich this experience with your dialogue, which can only result in benefits for us all.

### Cap Sensor Sensitivity ###
The MPR121 can measure capacitances ranging from 10 pF to 2000 pF (approx.) by varying the amount of charge current and charge time applied to the sensing inputs. This means that it may not be enough to go through certain materials. (See [AN3863 Designing Touch Sensing Electrodes](https://www.nxp.com/docs/en/application-note/AN3863.pdf)). In any case, charge current and charge time can be modified to reach other capacitance sensitivities.

At the end of the charge time, the electrode voltage is measured with a 10 bit ADC. The highest operating range of the electrode charging source is Vdd - 0.7V: it is critical not to charge the electrode beyond this point because it’s a non-linear region, meaning the ADC reading can quickly overflow returning meaningless results. For more information see [AN3889 Capacitance Sensing Settings](https://www.nxp.com/docs/en/application-note/AN3889.pdf).

This is why the Autoconfig feature is so sweet: it automatically tests various charge times and charge currents to determine the maximum values (within definable limits) that result in controlled output, and it does this *for each individual electrode*, allowing for differences in capacitance due to conector length, pad characteristics, etc.  Details on the input variables for Autoconfig start in Page 7 of [AN3889](https://www.nxp.com/docs/en/application-note/AN3889.pdf).

### MPR121 Library ###
To use Autoconfig and change calibration parameters during program execution, some serious modifications to the Adafruit_MPR121 library files are necessary, so make a copy of the .cpp and .h files into the file directory of your sketch and rename them to something like “my_MPR121.cpp”. The next time you open your sketch these two files will appear as tabs where you can modify them. Change the corresponding #include statements to the file name you chose. These actions will ensure that the original MPR121 library remains untouched.
