#include <pico/stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <etl/unordered_map.h>
#include <string.h>


etl::unordered_map<std::string, int, 3, 3, std::hash<std::string>>
lookup_table
{
    {"RED", 0},
    {"GREEN", 32},
    {"BLUE", 12}
};


char read_buffer[256] = {0}; // zero initialize array.
uint8_t buffer_index = 0;

void read_stdin_nonblocking()
{
    int16_t new_byte;
    while (true)
    {
        new_byte = getchar_timeout_us(0);
        if (new_byte == PICO_ERROR_TIMEOUT)
        {
            // Null-terminate end of a sequence so we can read partial cmds.
            read_buffer[buffer_index+1] = 0;
            return;
        }
        read_buffer[buffer_index++] = new_byte; // truncate to 8-bit char.
        // TODO: handle running into end-of-buffer.
    }
}

uint8_t chars_available()
{
    return buffer_index;
}

bool buffer_ends_with(uint8_t character)
{
    if (buffer_index == 0)
        return false;
    return read_buffer[buffer_index-1] == character;
}

bool buffer_ends_with(const char* text)
{
    uint8_t len = strlen(text);
    if (chars_available() < len)
        return false;
    return (strcmp(text, &read_buffer[buffer_index - len]) == 0);
}

char* read()
{
    buffer_index = 0;
    return &read_buffer[0];
}

char* read_until(const char* text)
{
    // Warning: destructively modifies string.
    char* index = strstr(read_buffer, text);
    *index = 0; // Null terminate at the match.
    return read();
}

int main()
{
// USB stdio init. Block until connected to the seral port.
    stdio_usb_init();
    stdio_set_translate_crlf(&stdio_usb, false); // Don't replace outgoing chars.
    while (!stdio_usb_connected()){}
    printf("Hello, from a Raspberry Pi Pico!");

    etl::unordered_map<std::string, int, 3, 3, std::hash<std::string>>::iterator it;
    char* buffered_str;

    while (true)
    {
        read_stdin_nonblocking();
        if (!chars_available())
            continue;
        if (!buffer_ends_with("\r"))
            continue;
        buffered_str = read_until("\r");
        it = lookup_table.find(std::string(buffered_str));
        if (it == lookup_table.end())
        {
            printf("Could not parse input: %s\r\n", buffered_str);
            continue;
        }
        printf("You entered: %s, which has value %d.\r\n", it->first.c_str(),
               it->second);
    }
}
