
# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

MTU=962
kissattach -m $MTU -l /dev/serial0 serial 192.168.100.102
ifconfig ax0 txqueuelen 3
# echo    5000 > /proc/sys/net/ipv4/neigh/ax0/retrans_time_ms
# echo 1200000 > /proc/sys/net/ipv4/neigh/ax0/base_reachable_time_ms
ifconfig ax0 pointopoint 192.168.100.101
ifconfig ax0 -arp
arp -H ax25 -s 192.168.100.101 MYCALL-8

sleep 5

# Remove the default route via wlan0 if it is there, continue if not

route del default || true

# Add the default route via the /dev/ax0 device to 192.168.100.101 

route add default gw 192.168.100.101 ax0

# check if we can ping - turn off wifi if successful.

# ping -n -c 1 192.168.100.101 && rfkill block wifi

ntpdate-debian
