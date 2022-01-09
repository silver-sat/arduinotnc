
# Note, run as root from /etc/rc.local
set -x

EXTERNAL_INTERFACE=wlan0

if [ -f /sys/class/net/eth1/operstate ]; then

  # create virtual serial port for virtual machines
  # must execute before satellite instance...

  socat pty,link=/dev/serial0,rawer tcp-listen:8000,fork,reuseaddr &
  sleep 10

  EXTERNAL_INTERFACE=eth0

fi

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

MTU=960
kissattach -m $MTU -l /dev/serial0 serial 192.168.100.101
ifconfig ax0 txqueuelen 3
ifconfig ax0 -arp
arp -H ax25 -s 192.168.100.102 MYCALL-9

# Assumes serial port connection is via /dev/ax0 and
# available connection (to the real internet) is /dev/wlan0.

iptables -A FORWARD -i ax0 -j ACCEPT
iptables --flush -t nat
iptables -t nat -I POSTROUTING -o ${EXTERNAL_INTERFACE} -j MASQUERADE

echo 1 > /proc/sys/net/ipv4/ip_forward

/etc/init.d/ntp stop
/etc/init.d/ntp start

stunnel /home/pi/.stunnel.conf >/home/pi/.stunnel.log 2>&1 &

axlisten ax0 -a -r -t >/home/pi/.ax0.log 2>&1 &

