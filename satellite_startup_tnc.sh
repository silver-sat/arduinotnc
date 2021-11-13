
# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

tncattach /dev/serial0 115200 --mtu 968 --noipv6 --noup --daemon
ifconfig tnc0 192.168.100.102 pointopoint 192.168.100.101
ifconfig tnc0 txqueuelen 3

# Remove the default route via wlan0 if it is there, continue if not

route del default || true

# Add the default route via the /dev/tnc0 device to 192.168.100.101 

route add default gw 192.168.100.101 tnc0

# check if we can ping - turn off wifi if successful.

# ping -n -c 1 192.168.100.101 && rfkill block wifi
