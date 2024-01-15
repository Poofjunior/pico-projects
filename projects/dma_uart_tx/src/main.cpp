#include <cstring>
#include <hardware/dma.h>
#include <string>
#include <pico/stdlib.h>

#define UART_TX_PIN (5)

uint dma_chan;

/**
 * \brief nonblocking way to dispatch uart characters.
 * \details assumes a global DMA channel has already been assigned.
 */
void dispatch_uart_stream(uart_inst_t* uart, const char* starting_address,
                          size_t word_count)
{
    // DMA channel will write data to the uart, paced by DREQ_TX.
    dma_channel_config conf = dma_channel_get_default_config(dma_chan);

    // Setup Sample Channel.
    channel_config_set_transfer_data_size(&conf, DMA_SIZE_8);
    channel_config_set_read_increment(&conf, true); // read from starting memory address.
    channel_config_set_write_increment(&conf, false); // write to fixed uart memorey address.
    channel_config_set_irq_quiet(&conf, true);
    // Pace data according to pio providing data.
    uint uart_dreq = (uart == uart0)? DREQ_UART0_TX : DREQ_UART1_TX;
    channel_config_set_dreq(&conf, uart_dreq);
    channel_config_set_enable(&conf, true);
    // Apply samp_chan_ configuration.
    dma_channel_configure(
        dma_chan,               // Channel to be configured
        &conf,                  // corresponding DMA config.
        &uart_get_hw(uart)->dr, // write (dst) address.
        starting_address,       // read (source) address.
        word_count,             // Number of word transfers i.e: len(string).
        true                    // Do start immediately.
    );

}

inline bool uart_is_busy()
{return dma_channel_is_busy(dma_chan);}


// Core0 main.
int main()
{
// Setup DMA
    dma_chan = dma_claim_unused_channel(true);
    // Setup UART TX for periodic transmission of the time at a low baudrate.
    uart_inst_t* uart_id = uart0;
    uart_init(uart_id, 115200);
    uart_set_hw_flow(uart_id, false, false);
    uart_set_fifo_enabled(uart_id, false); // Set FIFO size to 1.
    uart_set_format(uart_id, 8, 1, UART_PARITY_NONE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);

    std::string txt = "Hey there!";
    while(true)
    {
        if (uart_is_busy())
            continue;
        dispatch_uart_stream(uart_id, txt.c_str(), txt.size());
        sleep_ms(1000);
    }
}
