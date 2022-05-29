#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/sync.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE (4)
#define LOOP_INTERVAL_MS (20)

// Double buffer for writing data (Core1) and reading it (Core0).
uint32_t encoder_buffers[2][BUFFER_SIZE];
uint32_t* write_buffer_ptr = encoder_buffers[0];
uint32_t* read_buffer_ptr = encoder_buffers[1];


void core1_main()
{
    // Clear buffer data. (Only need to clear the read buffer.)
    for (auto i=0; i<BUFFER_SIZE; ++i)
        read_buffer_ptr[i] = 0;

    // Loop forever.
    // Populate write buffer with new data on set interval. Switch buffers.
    uint32_t* tmp;
    uint32_t scheduled_time_t = to_ms_since_boot(get_absolute_time());
    while (true)
    {
        // Wait until scheduled time.
        while(scheduled_time_t - to_ms_since_boot(get_absolute_time()) < 0){}
        // Set next scheduled time.
        scheduled_time_t += LOOP_INTERVAL_MS;
        // Write data.
        for (auto i=0; i<BUFFER_SIZE; ++i)
            write_buffer_ptr[i] = read_buffer_ptr[i] + 1;
        // Switch buffers.
        tmp = write_buffer_ptr;
        write_buffer_ptr = read_buffer_ptr;
        read_buffer_ptr = tmp;
    }
}


int main()
{
    // Local storage container for the data to read.
    uint32_t buffer_data[BUFFER_SIZE];
    uint32_t interrupted_status;

    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false);
    while (!stdio_usb_connected()){} // Wait until serial port connects.

    // launch core1.
    multicore_launch_core1(core1_main);

    while (true)
    {
        // Make a local copy so the data doesn't change while we're using it.
        // Guard with interrupts so we read the data out without delaying.
        interrupted_status = save_and_disable_interrupts();
        memcpy(buffer_data, read_buffer_ptr, sizeof(buffer_data));
        restore_interrupts(interrupted_status);

        printf("Buffer data: ");
        for (auto i=0; i<BUFFER_SIZE; ++i)
            printf("| %d |", buffer_data[i]);
        printf("\r\n");
        sleep_ms(500);
    }
}
