#include <stdint.h>
#ifndef __LIBNOW_H__
#define __LIBNOW_H__

typedef struct {
    int left_control;
    int right_control;
} message_control_status;

typedef enum { DST_ROBOT = 0, DST_MANDO } LibNowDst;

void libnow_init();

void libnow_addPeer(LibNowDst dst);
void libnow_sendMessage(LibNowDst dst, message_control_status* msg);

#endif