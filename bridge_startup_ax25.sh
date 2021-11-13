
# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

MTU=962
kissattach -m $MTU -l /dev/serial0 serial 192.168.100.101
ifconfig ax0 txqueuelen 3
# echo    5000 > /proc/sys/net/ipv4/neigh/ax0/retrans_time_ms
# echo 1200000 > /proc/sys/net/ipv4/neigh/ax0/base_reachable_time_ms
ifconfig ax0 pointopoint 192.168.100.102
ifconfig ax0 -arp
arp -H ax25 -s 192.168.100.102 MYCALL-9

# Assumes serial port connection is via /dev/ax0 and
# available connection (to the real internet) is /dev/wlan0.

iptables -A FORWARD -i ax0 -j ACCEPT
iptables --flush -t nat
iptables -t nat -I POSTROUTING -o wlan0 -j MASQUERADE

echo 1 > /proc/sys/net/ipv4/ip_forward

# axlisten ax0 -a 2>&1 > /home/pi/ax0.log &

ntpdate-debian
