#ifndef MAILBOX_H
#define	MAILBOX_H

#include <stdint.h>

#include "base.h"
/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

typedef struct {
    volatile uint8_t channel: 4;
    volatile uint32_t data: 28;
} mail_message_t;

typedef struct {
    volatile uint32_t reserved: 30;
    volatile uint8_t empty: 1;
    volatile uint8_t full:1;
} mail_status_t;

#define MAILBOX_BASE    PBASE + 0xb880

#define MAILBOX_READ   (mail_message_t*) (MAILBOX_BASE)
#define MAILBOX_STATUS (mail_status_t*) (MAILBOX_BASE + 0x18)
#define MAILBOX_WRITE  (mail_message_t*) (MAILBOX_BASE + 0x20)

#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

#endif