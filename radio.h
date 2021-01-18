
//
// Radio support and wrappers for basic functionality
//

#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3

RH_RF95 *rfm96w;
RHReliableDatagram *radio;

byte radiobuf[MSGSIZE+1];
byte radiolen=MSGSIZE;
byte radioflags=0;

void radio_setup() {
	
	rfm96w = new RH_RF95(RFM95_CS, RFM95_INT);
	
	// Initialize the radio
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

  radio = new RHReliableDatagram(*rfm96w, mynodeid());
  radio->setTimeout(RADIO_SEND_ACK_TIMEOUT);
  radio->setRetries(0);

	if (!radio->init()) {
		console("LoRa radio init failed");
		while (1);
	}
	if (!rfm96w->setFrequency(RADIO_FREQ)) {
		console("setFrequency failed");
		while (1);
	}
	rfm96w->setTxPower(RADIO_POWER, false);

  console("Radio ready.\n");
}

bool radio_send(byte *buffer, byte length, byte flags=0) {
  radio->setHeaderFlags(flags,0x0F);
  if (radio->sendtoWait(buffer, length, othernodeid())) {
	  console("  Radio send: %d bytes (success)\n",length);
    return true;
  }
  console("  Radio send: %d bytes (failed)\n",length);
  return false;
}

bool radio_receive() {
  radiolen=MSGSIZE;
  radioflags=0;
  if (radio->recvfromAckTimeout(radiobuf, &radiolen, RADIO_RECEIVE_TIMEOUT) && radiolen > 0) {
	  radioflags = radio->headerFlags();
	  radiobuf[radiolen] = '\0';
	  console("  Radio recv: %d bytes\n",radiolen);
    return true;
  }
  return false;	
}
