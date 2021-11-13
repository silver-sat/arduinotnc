# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

stty -F /dev/serial0 raw
pppd /dev/serial0 115200 192.168.100.102:192.168.100.101 \
     defaultroute noauth local noccp mtu 440 mru 440 \
     persist maxfail 0 holdoff 1

sleep 10

# Remove the default route (for wlan0) if it is there, continue if not

# route del default || true

# Add the default route via the /dev/ppp0 device to 192.168.100.101

# route add default gw 192.168.100.101 ppp0

# check if we can ping - turn off wifi if successful.

# ping -n -c 1 192.168.100.101 && rfkill block wifi
