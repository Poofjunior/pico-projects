Example on how to use Alarms while bypassing the alarm pool.

## Environment Setup
Ensure that all git submodules have been cloned locally.
(See the top-level readme for details.)

## Compiling
From this directory, create an empty directory called **build**, enter it, and invoke cmake witheither:

````
mkdir build
cd build
cmake -DPICO_SDK_PATH=/path/to/pico-sdk ..
````
or (if the `PICO_SDK_PATH` is defined as an environment variable:
````
mkdir build
cd build
cmake ..
````
After this point, you can invoke the auto-generated Makefile with `make`.


## References
* [RP2040 Datasheet pg 537](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
