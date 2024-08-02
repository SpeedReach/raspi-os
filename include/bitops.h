#ifndef BITOPS_H
#define BITOPS_H

#define BITS_PER_BYTE 8

// Macro to calculate the number of bytes needed to store a given number of bits
#define NUM_BYTES(bits) (((bits) + BITS_PER_BYTE - 1) / BITS_PER_BYTE)

// Macros to set, clear, and check bits in the bit array
#define SET_BIT(array, n) ((array)[(n) / BITS_PER_BYTE] |= (1U << ((n) % BITS_PER_BYTE)))
#define CLEAR_BIT(array, n) ((array)[(n) / BITS_PER_BYTE] &= ~(1U << ((n) % BITS_PER_BYTE)))
#define CHECK_BIT(array, n) (((array)[(n) / BITS_PER_BYTE] & (1U << ((n) % BITS_PER_BYTE))) != 0)

#endif
