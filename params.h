
#define CONSOLE 1

#define RPISERIAL      Serial3
#define SERIALBAUD     115200
#define SERIALTIMEOUT  5

#define RADIO_FREQ               433.0
#define RADIO_POWER              5
#define RADIO_SEND_ACK_TIMEOUT   50
#define RADIO_RECEIVE_TIMEOUT	   5

#define MTU 512
#define MAX_KISS_FRAME (MTU+64)
#define MSGSIZE		200
#define MAX_ATTEMPTS 10

#define SEND_DELAY         100
#define SEND_FAIL_DELAY    250  
#define RECEIVE_SEND_DELAY 200
#define INTERFRAME_DELAY   200
