
#ifndef _PARAMS_H_
#define _PARAMS_H_

#define RPISERIAL      Serial3
#define SERIALBAUD     115200
#define SERIALTIMEOUT  5
#define RPIREADBUFFERSIZE 4096

#define RADIO_FREQ               435.0
#define RADIO_POWER              5
// #define RADIO_SEND_ACK_TIMEOUT   50
#define RADIO_RECEIVE_TIMEOUT	   5

#define MTU 960
#define MAX_KISS_FRAME (MTU+40)
#define MSGSIZE		250
// #define MAX_ATTEMPTS 10

#define RPIWRITEBUFFERSIZE MAX_KISS_FRAME

#define SEND_DELAY         10
// #define SEND_FAIL_DELAY    250
#define RECEIVE_DELAY      50
#define INTER_FRAME_DELAY  500

#endif
