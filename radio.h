
#ifndef _RADIO_H_
#define _RADIO_H_

#include <Arduino.h>

#include "types.h"

#include <RH_RF95.h>
#include <RH_RF69.h>
#include <RHDatagram.h>
#include <RHReliableDatagram.h>

#include "timer.h"
#include "kiss.h"

class BaseLink {
  protected:
    ulong _receive_timeout;
  public:
    virtual bool recv_msg(byte* buffer, byte &length, byte &flags)=0;
    virtual bool send_msg(const byte *buffer, const byte length, byte flags = 0)=0;
    void receive_timeout(ulong timeout) {
      _receive_timeout = timeout;
    };
    ulong receive_timeout() {
      return _receive_timeout;
    };
};

class RadioLink: virtual public BaseLink {
  protected:
    float _freq;
    uint _power;
    
    RHGenericDriver *_radio;

  public:
    RadioLink() {
      // sensible? defaults
      _freq = 435.0;
      _power = 5;
      _receive_timeout = 5;
      _radio = NULL;
    }
    void frequency(float f) {
      _freq = f;
    };
    void power(uint p) {
      _power = p;
    };
    RHGenericDriver *radio() {
      return _radio;
    }
    virtual bool recv_msg(byte* buffer, byte &length, byte &flags) {
      if (_radio->waitAvailableTimeout(_receive_timeout)) {
        if (_radio->recv(buffer, &length) && (length) > 0) {
           #ifdef HAS_CONSOLE
             console.printf("Radio receive: %d bytes\n", length);
           #endif
           return true;
        }
      }
      return false;
    }
    virtual bool send_msg(const byte *buffer, const byte length, byte flags = 0) {
      _radio->setHeaderFlags(flags, 0x0F);
      _radio->send(buffer, length);     
      _radio->waitPacketSent();
      #ifdef HAS_CONSOLE
        console.printf("Radio send: %d bytes\n", length);
      #endif
      return true;
    }
};

class RFM95_RadioLink: virtual public RadioLink {
  private:
    byte RFM95_CS = 4;
    byte RFM95_RST = 2;
    byte RFM95_INT = 3;

  public:
    void initialize() {
      RH_RF95 *radio = new RH_RF95(RFM95_CS, RFM95_INT);
      
      pinMode(RFM95_RST, OUTPUT);
      digitalWrite(RFM95_RST, HIGH);
      delay(10);
      digitalWrite(RFM95_RST, LOW);
      delay(10);
      digitalWrite(RFM95_RST, HIGH);
      delay(10);
      
      if (!radio->init()) {
        #ifdef HAS_CONSOLE
          console.printf("LoRa radio init failed");
        #endif
        while (1);
      }
      
      if (!radio->setFrequency(_freq)) {
        #ifdef HAS_CONSOLE
          console.printf("setFrequency failed");
        #endif
        while (1);
      }
      
      radio->setTxPower(_power, false);

      #ifdef HAS_CONSOLE
        console.printf("RFM95 initialized.\n");
      #endif

      _radio = radio;
    }   
};

class DatagramLink: virtual public BaseLink {
  protected:
    byte _myid;
    byte _otherid;
  public:
    DatagramLink() {
#ifdef HAS_NODEID
      _myid = nodeid.me();
      _otherid = nodeid.them();
#else
      _myid = 0;
      _otherid = 0;
#endif
    };
    void my_nodeid(byte id) {
      _myid = id;
    };
    void other_nodeid(byte id) {
      _otherid = id;
    };

};

class UnreliableDatagramLink: virtual public DatagramLink  {
    RHDatagram *_radio;    
  public:
    UnreliableDatagramLink() {};
    void initialize(RadioLink & link) {
      this->receive_timeout(link.receive_timeout());
      RHDatagram *radio = new RHDatagram(*(link.radio()), _myid);
      radio->init();
      _radio = radio;
    }
    // RHDatagram doesn't have the equivalent of recvfromTimeout, so take corresponding code from RHReliableDatagram
    bool recv_msg(byte* buffer, byte &length, byte &flags) {
      unsigned long timeout = _receive_timeout;
      unsigned long starttime = millis();
      int32_t timeLeft;
      while ((timeLeft = timeout - (millis() - starttime)) > 0) {
        if (_radio->waitAvailableTimeout(timeLeft)) {
          if (_radio->recvfrom(buffer, &length)) {
            flags = _radio->headerFlags();
            if (length > 0) {
              #ifdef HAS_CONSOLE
                console.printf("Radio received: %d bytes, flags %d\n", length, flags);
              #endif
              return true;
            } else {
              return false;
            }
          }
          YIELD;
        }
      }
      return false;
    }
    bool send_msg(const byte *buffer, const byte length, byte flags = 0) {
      _radio->setHeaderFlags(flags, 0x0F);
      _radio->sendto((byte *)buffer, length, _otherid);
      _radio->waitPacketSent();
      #ifdef HAS_CONSOLE
      console.printf("Radio send: %d bytes, flags %d\n", length, flags);
      #endif
      return true;
    }
};

class ReliableDatagramLink: virtual public DatagramLink  {
    RHReliableDatagram *_radio;    
    uint _send_ack_timeout;
  public:
    ReliableDatagramLink() {
      // sensible defaults?
      _send_ack_timeout = 50;
    };
    void initialize(RadioLink & link) {
      this->receive_timeout(link.receive_timeout());
      RHReliableDatagram *radio = new RHReliableDatagram(*(link.radio()), _myid);
      radio->setTimeout(_send_ack_timeout);
      radio->setRetries(0);
      radio->init();
      _radio = radio;
    }
    void send_ack_timeout(uint timeout) {
      _send_ack_timeout = timeout;
    };
    bool recv_msg(byte* buffer, byte &length, byte &flags) {
      if (_radio->recvfromAckTimeout(buffer, &length, _receive_timeout) && length > 0) {
        flags = _radio->headerFlags();
        #ifdef HAS_CONSOLE
          console.printf("Radio received: %d bytes, flags %d\n", length, flags);
        #endif
        return true;
      }
      return false;
    }
    bool send_msg(const byte *buffer, const byte length, byte flags = 0) {
      _radio->setHeaderFlags(flags, 0x0F);
      if (_radio->sendtoWait((byte *)buffer, length, _otherid)) {
        #ifdef HAS_CONSOLE
          console.printf("Radio send: %d bytes, flags %d (success)\n", length, flags);
        #endif
        return true;
      }
      #ifdef HAS_CONSOLE
        console.printf("Radio send: %d bytes, flags %d (failed)\n", length, flags);
      #endif
      return false;
    }
};

class MultiMessageRadioLink {
  protected:
    BaseLink* _radio;
    
    // buffer for receiving data-packet
    byte *_buffer;
    uint _bufferlen;

    // details of current send data-packet
    const byte *_sendbuffer;
    uint _sendbuflen;
    ulong _sentpackets;

    // details of current send messages
    uint _sendmsgind;
    uint _sendmsgatt;

    // details of current messages received
    uint _recvmsgind;
    byte _recvmsgid;
    byte _recvdatid;
    bool _recvdat;

    duration _send_delay;
    duration _receive_delay;
    duration _send_fail_delay;
    duration _last_msg_delay;
    uint _max_attempts;
    uint _msg_size;

    Timer _send_timer;

  public:
    MultiMessageRadioLink(byte *buffer,
                          const uint bufferlen) {
      _send_delay = 100;
      _receive_delay = 200;
      _send_fail_delay = 250;
      _max_attempts = 10;
      _msg_size = 200;
      _last_msg_delay = 100;
      
      _buffer = buffer;
      _bufferlen = bufferlen;
    };
    MultiMessageRadioLink(const uint bufferlen) {
      _send_delay = 100;
      _receive_delay = 200;
      _send_fail_delay = 250;
      _max_attempts = 10;
      _msg_size = 200;
      _last_msg_delay = 100;
      
      _buffer = new byte[bufferlen];
      _bufferlen = bufferlen;
    };
    void initialize(BaseLink & radio) {
      _radio = &radio;
      
      // initialize state variables for send and receive...
      _sentpackets = 0;
      _sendbuffer = 0;
      _recvdat = false;
      _recvmsgind = 0;
    };
    void send_delay(duration d) {
      _send_delay = d;
    };
    void receive_delay(duration d) {
      _receive_delay = d;
    };
    void send_fail_delay(duration delay) {
      _send_fail_delay = delay;
    };
    void max_attempts(uint attempts) {
      _max_attempts = attempts;
    };
    void message_size(uint s) {
      _msg_size = s;
    };
    void last_msg_delay(duration delay) {
      _last_msg_delay = delay;
    };
    byte flag_value() {
      byte pktid = _sentpackets & 0x03;
      byte msgid = _sendmsgind & 0x03;
      return ((pktid << 2) | msgid);
    }
    bool receive(byte* &buffer, uint &length) {
      uint usedbytes = _recvmsgind * _msg_size;
      byte *radiobuf = _buffer + usedbytes;
      byte radiolen = _msg_size;
      if ((_bufferlen - usedbytes) < radiolen) {
        radiolen = (byte)(_bufferlen - usedbytes);
      }
      byte radioflags;
      if (_radio->recv_msg(radiobuf, radiolen, radioflags)) {
        bool retval = false;
        bool first = (radiobuf[0] == FEND);
        bool last = ((radiobuf[radiolen - 1] == FEND) || (radiolen < _msg_size));
        byte datid = (radioflags >> 2) & 0x03;
        byte msgid = (radioflags & 0x03);
#ifdef HAS_CONSOLE
        if (_recvdat) {
          console.printf("Received %d-%d (%d-%d) len %d first %d last %d\n", datid, msgid, _recvdatid, _recvmsgid, radiolen, 1 * first, 1 * last);
        } else {
          console.printf("Received %d-%d (x-x) len %d first %d last %d\n", datid, msgid, radiolen, 1 * first, 1 * last);
        }
#endif
        if (first || !_recvdat || (!_recvdat && datid != _recvdatid)) {
          if (_recvdat) {
#ifdef HAS_CONSOLE
            console.printf("Bad msg dropped, pkt id %d to be dropped\n", _recvdatid);
#endif
          }
          _recvdat = true;
          _recvdatid = datid;
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
            _recvmsgind = 0;
            retval = true;
          }
        } else {
#ifdef HAS_CONSOLE
          console.printf("Bad msg dropped, pkt id %d to be dropped\n", _recvdatid);
#endif
          _recvdat = false;
        }
        // Delay any other sends to give a chance for more radio messages
        // console.printf("receive_delay %lu\n",_receive_delay);
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
        if (_radio->send_msg(_sendbuffer + msgstart, msglen, flags)) {
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
          _send_timer.random_wait(min(10000, _send_fail_delay * pow(2, _sendmsgatt)));
        }
      }
      return false;
    };

    void send(const byte *buffer, const uint length) {
      _sendbuffer = buffer;
      _sendbuflen = length;
      _sendmsgind = 0;
      _sendmsgatt = 0;
      _send_timer.wait(_send_delay);
    };

    bool ready() {
      return (_sendbuffer == 0);
    }
    bool busy() {
      return (_sendbuffer != 0);
    }
};

#endif
