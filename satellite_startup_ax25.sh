
# Note, run as root from /etc/rc.local
# % cd /home/pi
# % rm -f .startup.sh
# % ln -s satellite_startup_ax25.sh .startup.sh
#
# Altenatively, you can run it manually if /etc/rc.local is not setup.
# % sudo sh .startup.sh >& .startup.log
#

set -x

echo -n "Up since: "; uptime -s

if [ -e /sys/class/net/eth1/operstate ]; then

  echo -n "Now: "; date

  # Set up virtual serial port for virtual raspberry pi
  # change 192.254.192.195 to be the address of the "internal" virtual 
  # box interface (eth1) on the other raspberry pi (virtual bridge)

  RATE=400
  BRIDGE_ETH1_IP=169.254.192.195
 
  until ping -n -c 1 ${BRIDGE_ETH1_IP} >/dev/null 2>&1; do
    sleep 5
  done

  echo "$RATE" > /home/pi/.rate.txt
  socat pty,link=/dev/serial0,rawer exec:"/home/pi/.slowsocat.sh ${BRIDGE_ETH1_IP} 8000 ${RATE}" &

  until [ -e /dev/serial0 ]; do
    sleep 5
  done

fi

echo -n "Now: "; date

# Commands at boot for satellite raspberry pi: 
# GS/Bridge is 192.168.100.101
# Satellite is 192.168.100.102

MTU=960
kissattach -m $MTU -l /dev/serial0 serial 192.168.100.102
ifconfig ax0 txqueuelen 3
ifconfig ax0 -arp
arp -H ax25 -s 192.168.100.101 MYCALL-8

axlisten ax0 -a -r -t >/home/pi/.ax0.log 2>&1 &

# Remove the default route if it is there, continue if not

route del default || true

# Add the default route via the /dev/ax0 device to 192.168.100.101 

route add default gw 192.168.100.101 ax0

# check if we can ping - turn off wifi if successful.

# ping -n -c 1 192.168.100.101 && rfkill block wifi

# set our clock based on the time-server on the bridge rpi
# Note, have to allow 192.168.100.102 in /etc/ntp.conf on bridge rpi

echo -n "Now: "; date

ntpdate -u 192.168.100.101

echo -n "Up since: "; uptime -s
echo -n "Now: "; date

# Use: touch /home/pi/.dopayload to execute payload.sh at next reboot. 
if [ -f /home/pi/.dopayload ]; then
  rm -f /home/pi/.dopayload
  echo -n "Now: "; date
  runuser -u pi /home/pi/payload.sh > /home/pi/payload.log 2>&1
  echo -n "Now: "; date
  shutdown now
fi
