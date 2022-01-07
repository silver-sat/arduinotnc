
# Note, run as root from /etc/rc.local
set -x

EXTERNAL_INTERFACE=wlan0

if [ -f /sys/class/net/eth1/operstate ]; then

  # create virtual serial port for virtual machines
  # must execute before satellite instance...

  RATE=400
  # socat pty,link=/dev/serial0,rawer,ispeed=${BAUD},ospeed=${BAUD},b${BAUD} tcp-listen:8000,fork &
  socat pty,link=/dev/serial0,rawer exec:"/home/pi/.slowsocat.sh 8000 ${RATE}" &
  sleep 10

  EXTERNAL_INTERFACE=eth0

fi

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

MTU=960
kissattach -m $MTU -l /dev/serial0 serial 192.168.100.101
ifconfig ax0 txqueuelen 3
# echo    5000 > /proc/sys/net/ipv4/neigh/ax0/retrans_time_ms
# echo 1200000 > /proc/sys/net/ipv4/neigh/ax0/base_reachable_time_ms
# ifconfig ax0 pointopoint 192.168.100.102
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

