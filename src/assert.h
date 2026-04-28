#pragma once
#include <Arduino.h>

#define ASSERT(cond) do { if (!(cond)) { Serial.println(F("ASSERT FAIL")); while (1) {} } } while (0)
