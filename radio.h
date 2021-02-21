
#ifndef _RADIO_H_
#define _RADIO_H_

#include <Arduino.h>

#include "types.h"

#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#include "timer.h"
#include "kiss.h"

class RadioLink {
  protected:
    float _freq;
    uint _power;
    uint _receive_timeout;
    uint _send_delay;
    uint _receive_delay;
    Timer _send_timer;

  public:
    RadioLink() {
      // sensible? defaults
      _freq = 435.0;
      _power = 5;
      _receive_timeout = 5;
      _send_delay = 100;
      _receive_delay = 200;
    }
    void frequency(float f) {
      _freq = f;
    };
    void power(uint p) {
      _power = p;
    };
    void receive_timeout(uint timeout) {
      _receive_timeout = timeout;
    };
    void send_delay(uint delay) {
      _send_delay = delay;
    };
    void receive_delay(uint delay) {
      _receive_delay = delay;
    };
    Timer & send_timer() {
      return _send_timer;
    };
};

class ReliableRadioLink: public RadioLink {
  protected:
    uint _send_ack_timeout;
    uint _send_fail_delay;
    byte _myid;
    byte _otherid;
    uint _max_attempts;
  public:
    ReliableRadioLink() {
      // sensible defaults?
      _send_ack_timeout = 50;
      _send_fail_delay = 250;
#ifdef HAS_NODEID
      _myid = nodeid.me();
      _otherid = nodeid.them();
#else
      _myid = 0;
      _otherid = 0;
#endif
      _max_attempts = 10;
    };
    void send_ack_timeout(uint timeout) {
      _send_ack_timeout = timeout;
    };
    void send_fail_delay(uint delay) {
      _send_fail_delay = delay;
    };
    void max_attempts(uint attempts) {
      _max_attempts = attempts;
    };
    void my_nodeid(byte id) {
      _myid = id;
    };
    void other_nodeid(byte id) {
      _otherid = id;
    };
};

class MultiMessageReliableRadioLink: public ReliableRadioLink {
  protected:
    uint _msg_size;
    uint _last_msg_delay;
  public:
    MultiMessageReliableRadioLink() {
      _msg_size = 200;
      _last_msg_delay = _send_delay;
    };
    void message_size(uint s) {
      _msg_size = s;
    };
    void last_msg_delay(uint delay) {
      _last_msg_delay = delay;
    };
};

class MultiMessageReliableRFM95Link: public MultiMessageReliableRadioLink {
  private:
    byte RFM95_CS = 4;
    byte RFM95_RST = 2;
    byte RFM95_INT = 3;

    RH_RF95 *_rfm96w;
    RHReliableDatagram *_radio;

    // buffer for receiving data-packet
    byte *_buffer;
    uint _bufferlen;

    // details of current send data-packet
    const byte *_sendbuffer;
    uint _sendbuflen;
    uint _sentpackets;

    // details of current send messages
    byte _sendmsgind;
    byte _sendmsgatt;

    // details of current messages received
    byte _recvmsgind;
    byte _recvmsgid;
    byte _recvdatid;
    bool _recvdat;

  public:
    MultiMessageReliableRFM95Link(byte *buffer,
                                  const uint bufferlen) {
      _buffer = buffer;
      _bufferlen = bufferlen;
    };
    MultiMessageReliableRFM95Link(const uint bufferlen) {
      _buffer = new byte[bufferlen];
      _bufferlen = bufferlen;
    };
    void initialize() {
      _rfm96w = new RH_RF95(RFM95_CS, RFM95_INT);
      pinMode(RFM95_RST, OUTPUT);
      digitalWrite(RFM95_RST, HIGH);
      delay(10);
      digitalWrite(RFM95_RST, LOW);
      delay(10);
      digitalWrite(RFM95_RST, HIGH);
      delay(10);
      _radio = new RHReliableDatagram(*_rfm96w, _myid);
      _radio->setTimeout(_send_ack_timeout);
      _radio->setRetries(0);

      if (!_radio->init()) {
#ifdef HAS_CONSOLE
        console.printf("LoRa radio init failed");
#endif
        while (1);
      }
      if (!_rfm96w->setFrequency(_freq)) {
#ifdef HAS_CONSOLE
        console.printf("setFrequency failed");
#endif
        while (1);
      }
      _rfm96w->setTxPower(_power, false);

      // initialize state variables for send and receive...
      _sentpackets = 0;
      _sendbuffer = 0;
      _recvdat = false;

#ifdef HAS_CONSOLE
      console.printf("Radio ready.\n");
#endif
    };

    byte flag_value() {
      byte pktid = _sentpackets & 0x03;
      byte msgid = _sendmsgind & 0x03;
      return ((pktid << 2) | msgid);
    }

    bool receive(byte* &buffer, uint &length) {
      byte *radiobuf = _buffer + _recvmsgind * _msg_size;
      byte radiolen;
      byte radioflags;
      if (recv_msg(radiobuf, radiolen, radioflags)) {
        bool retval = false;
        bool first = (radiobuf[0] == FEND);
        bool last = (radiobuf[radiolen - 1] == FEND);
        byte datid = (radioflags >> 2) & 0x03;
        byte msgid = (radioflags & 0x03);
#ifdef HAS_CONSOLE
        console.printf("Received %d-%d (%d-%d) len %d first %d last %d\n", datid, msgid, _recvdatid, _recvmsgid, radiolen, 1 * first, 1 * last);
#endif
        if (first || !_recvdat || (!_recvdat && datid != _recvdatid)) {
          if (_recvdat) {
#ifdef HAS_CONSOLE
            console.printf("Bad msg dropped, pkt id %d to be dropped\n", _recvdatid);
#endif
          }
          _recvdat = true;
          _recvdatid = datid;
          _recvmsgind = 0;
          _recvmsgid = (_recvmsgind & 0x03);
        }
        if (datid == _recvdatid && msgid == _recvmsgid) {
          if (!last) {
            _recvmsgind += 1;
            _recvmsgid = (_recvmsgind & 0x03);
          } else {
            buffer = _buffer;
            length = _recvmsgind * _msg_size + radiolen;
            _recvdat = false;
            retval = true;
          }
        } else {
#ifdef HAS_CONSOLE
          console.printf("Bad msg dropped, pkt id %d to be dropped\n", _recvdatid);
#endif
          _recvdat = false;
        }
        // Delay any other sends to give a chance for more radio messages
        _send_timer.wait(_receive_delay);
        return retval;
      } else if (_sendbuffer != 0 && _send_timer.ready()) {
        uint msgstart = _sendmsgind * _msg_size;
        uint msglen = _sendbuflen - msgstart;
        bool last = true;
        if (msglen > _msg_size) {
          msglen = _msg_size;
          last = false;
        }
        byte flags = flag_value();
#ifdef HAS_BLINKER
        blinker.blink();
#endif
        if (send_msg(_sendbuffer + msgstart, msglen, flags)) {
          if (!last) {
            _sendmsgind += 1;
            _sendmsgatt = 0;
            _send_timer.wait(_send_delay);
          } else {
            _sendbuffer = 0;
            _sentpackets += 1;
            _send_timer.wait(_last_msg_delay);
          }
        } else {
          _sendmsgatt += 1;
          if (_sendmsgatt < _max_attempts) {
            _send_timer.wait(_send_delay);
          } else {
            _sendbuffer = 0;
            _send_timer.wait(_last_msg_delay);
          }
          _send_timer.random_wait(_send_fail_delay * pow(2, _sendmsgatt));
        }
      }
      return false;
    };

    bool recv_msg(byte* radiobuf, byte &radiolen, byte &radioflags) {
      radiolen = _msg_size; // what about the last message?
      radioflags = 0;
      if (_radio->recvfromAckTimeout(radiobuf, &radiolen, _receive_timeout) && radiolen > 0) {
        radioflags = _radio->headerFlags();
#ifdef HAS_CONSOLE
        console.printf("  Radio recv: %d bytes\n", radiolen);
#endif
        return true;
      }
      return false;
    }

    bool send_msg(const byte *buffer, const uint length, byte flags = 0) {
      _radio->setHeaderFlags(flags, 0x0F);
      if (_radio->sendtoWait((byte *)buffer, length, _otherid)) {
#ifdef HAS_CONSOLE
        console.printf("  Radio send: %d bytes (success)\n", length);
#endif
        return true;
      }
#ifdef HAS_CONSOLE
      console.printf("  Radio send: %d bytes (failed)\n", length);
#endif
      return false;
    }

    void send(const byte *buffer, const uint length) {
      _sendbuffer = buffer;
      _sendbuflen = length;
      _sendmsgind = 0;
      _sendmsgatt = 0;
    };

    bool ready() {
      return (_sendbuffer == 0);
    }
    bool busy() {
      return (_sendbuffer != 0);
    }
};

#endif
