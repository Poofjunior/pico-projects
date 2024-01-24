#include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>

void alarm_callback_fn(uint alarm_num)
{
    gpio_put(25, 1); // Turn on LED from callback.
}

int main()
{
    //stdio_usb_init();
    //stdio_set_translate_crlf(&stdio_usb, false); // Don't replace outgoing chars.
    //while (!stdio_usb_connected()){} // Block until connection to serial port.

    gpio_init(25);
    gpio_set_dir(25, true); // output
    gpio_put(25, 0);

    int alarm_num = hardware_alarm_claim_unused(true); // required = true;
    hardware_alarm_set_target(alarm_num, time_us_64() + 1000000); // Set 1 sec from now.
    hardware_alarm_set_callback(alarm_num, alarm_callback_fn); // returns true if we missed the target.

    // TODO: do we need to: hardware_alarm_unclaim ??

    while(true){}
}
