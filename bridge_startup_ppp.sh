# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

stty -F /dev/serial0 raw
pppd /dev/serial0 115200 192.168.100.101:192.168.100.102 \
      proxyarp local noauth noccp mtu 440 mru 440 \
      passive persist maxfail 0 holdoff 1

# Assumes serial port connection is via /dev/ppp0 and
# available connection (to the real internet) is /dev/wlan0.

iptables -A FORWARD -i ppp0 -j ACCEPT
iptables --flush -t nat
iptables -t nat -I POSTROUTING -o wlan0 -j MASQUERADE

echo 1 > /proc/sys/net/ipv4/ip_forward
