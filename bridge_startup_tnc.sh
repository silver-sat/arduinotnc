
# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

tncattach /dev/serial0 115200 --mtu 968 --noipv6 --noup --daemon
ifconfig tnc0 192.168.100.101 pointopoint 192.168.100.102
ifconfig tnc0 txqueuelen 3 

# Assumes serial port connection is via /dev/ax0 and
# available connection (to the real internet) is /dev/wlan0.

iptables -A FORWARD -i tnc0 -j ACCEPT
iptables --flush -t nat
iptables -t nat -I POSTROUTING -o wlan0 -j MASQUERADE

echo 1 > /proc/sys/net/ipv4/ip_forward
