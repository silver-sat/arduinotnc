
# Note, run as root from /etc/rc.local
set -x

# Commands at boot for bridge raspberry pi: Bridge is 192.168.100.101
# Connected raspberry pi is 192.168.100.102

kissattach -l /dev/serial0 serial 192.168.100.102

# Remove the default route via wlan0 if it is there, continue if not

# route del default || true

# Add the default route via the /dev/ax0 device to 192.168.100.101 

# route add default gw 192.168.100.101 ax0

# check if we can ping - turn off wifi if successful.

# ping -n -c 1 192.168.100.101 && rfkill block wifi
