#pragma once
#include <stdint.h>

struct SerialReader {
    uint8_t pos;
};

bool checkSerial(SerialReader* reader, char* buf, uint8_t buf_size);
