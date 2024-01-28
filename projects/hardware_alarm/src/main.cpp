#include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <hardware/timer.h>

uint32_t alarm_num = 0;


inline uint32_t hardware_alarm_irq_number(uint32_t alarm_num)
{
    return TIMER_IRQ_0 + alarm_num;
}

void alarm_callback_fn(void)
{
    gpio_put(25, !gpio_get(25)); // Toggle the LED.
    // Clear the latched hardware interrupt.
    timer_hw->intr |= (1u << alarm_num);
    // rearm the alarm.
    timer_hw->alarm[alarm_num] = (time_us_32() + uint32_t(1000000)); // write time (also arms the alarm)

}

int main()
{
    //stdio_usb_init();
    //stdio_set_translate_crlf(&stdio_usb, false); // Don't replace outgoing chars.
    //while (!stdio_usb_connected()){} // Block until connection to serial port.

    gpio_init(25);
    gpio_set_dir(25, true); // output
    gpio_put(25, 0);

    //int alarm_num = hardware_alarm_claim_unused(true); // required = true;
    //hardware_alarm_set_target(alarm_num, time_us_64() + 1000000); // Set 1 sec from now.
    //hardware_alarm_set_callback(alarm_num, alarm_callback_fn); // returns true if we missed the target.

    // Note: does not handle edge case if we schedule alarm to happen *now*
    // Attach the interrupt to the function.
    uint32_t irq_num = hardware_alarm_irq_number(alarm_num);
    irq_set_exclusive_handler(irq_num, alarm_callback_fn); // attach interrupt to function.
    irq_set_enabled(irq_num, true); // enable alarm0 interrupt

    // Arm the alarm.
    timer_hw->inte |= (1u << alarm_num); // enable specified alarm to trigger interrupt.
    timer_hw->alarm[alarm_num] = (time_us_32() + uint32_t(1000000)); // write time (also arms the alarm)

    while(true){}

    // Optional: detach this handler function so we can use the same alarm
    // on a different handler function.
    //uint irq_num = hardware_alarm_irq_number(alarm_num);
    //irq_remove_handler(irq_num, alarm_callback_fn);
}
