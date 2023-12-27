Example on how to read the systick register.

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

## Scheduling Overview
Todo!


## References
* [Pi Forum Post: Systick](https://forums.raspberrypi.com/viewtopic.php?t=304201)
