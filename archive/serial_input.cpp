#include "serial_input.h"
#include "assert.h"
#include <Arduino.h>

bool checkSerial(SerialReader* reader, char* buf, uint8_t buf_size) {
    ASSERT(reader != nullptr);
    ASSERT(buf != nullptr);
    ASSERT(buf_size >= 2);

    for (uint8_t i = 0; i < 64 && Serial.available(); i++) {
        int c_in = Serial.read();
        ASSERT(c_in >= 0);
        char c = (char)c_in;
        if (c == '\n' || c == '\r') {
            if (reader->pos == 0) continue;
            ASSERT(reader->pos < buf_size);
            buf[reader->pos] = '\0';
            reader->pos = 0;
            return true;
        }
        if (reader->pos < buf_size - 1) {
            ASSERT(reader->pos < buf_size);
            buf[reader->pos++] = c;
        }
    }
    return false;
}
