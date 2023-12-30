#include "peripherals/mailbox.h"
#include "printf.h"
#include "utils.h"

#define GET_ARM_MEMORY 0x00010005
#define GET_BOARD_REVISION  0x00010002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000



int mailbox_read(int channel){
  mail_status_t status;
  for(;;){
    while (1){
      status = *MAILBOX_STATUS;
      if(!status.empty){
        break;
      }
    }
    mail_message_t mes = * MAILBOX_READ;
	  if (mes.channel == channel)
		  return mes.data;
  }
}

void mailbox_write(mail_message_t mes){
  mail_status_t status;
  while (1){
    status = *MAILBOX_STATUS;
    if(!status.full){
      break;
    }
  }
  *MAILBOX_WRITE = mes;
}

int mailbox_call(volatile uint32_t* mAdr,int channel){
  mail_message_t mes;
  mes.data =((int)  mAdr) >> 4;
  mes.channel = channel;
  mailbox_write(mes);
  if(mes.data == mailbox_read(channel) && mAdr[1] == REQUEST_SUCCEED){
    return 1;
  }
  return 0;
}

void print_board_revision(){
  unsigned int mailbox[7];
  mailbox[0] = 7 * sizeof(unsigned int); // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_BOARD_REVISION; // tag identifier
  mailbox[3] = 4; // maximum of request and response value buffer's length.
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0; // value buffer
  // tags end
  mailbox[6] = END_TAG;
  mailbox_call(mailbox, MBOX_CH_PROP);
  printf("board revision: 0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
}

void print_arm_memory(){
    unsigned int mailbox[8];
    mailbox[0] = 7 * sizeof(unsigned int);
    mailbox[1] = REQUEST_CODE;
    mailbox[2] = GET_ARM_MEMORY;
    mailbox[3] = 8;
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[7] = END_TAG;
    mailbox_call(mailbox, MBOX_CH_PROP);
    printf("mem base %x, size %x\n", mailbox[5],mailbox[6]);
}