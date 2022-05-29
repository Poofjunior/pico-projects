#include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define SYST_CSR (*(volatile uint32_t*)(PPB_BASE + 0xe010))
#define SYST_CVR (*(volatile uint32_t*)(PPB_BASE + 0xe018))

int main()
{
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false); // Don't replace outgoing chars.
    while (!stdio_usb_connected()){} // Block until connection to serial port.


    SYST_CSR |= (1 << 2) | (1 << 0); // user processor clock. 133MHz by default.
    //SYST_CSR |= (1 << 0); // use external clock. This appears to be in microseconds?

    while(true)
    {
        printf("SYSTICK Register: %d\r\n",SYST_CVR);
        sleep_ms(1);
    }
}
