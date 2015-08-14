#include <avr/io.h>

FUSES = {
    .low = LFUSE_DEFAULT,
    .high = HFUSE_DEFAULT,
    #ifndef __AVR_ATmega328P__
    .extended = EFUSE_DEFAULT
    #endif // __AVR_ATmega328P__
};
