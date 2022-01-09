# Setup instructions for Raspberry PIs

The two RPis will be called bridge, with IP address 192.168.100.101 and callsign MYCALL-8, and satellite, with IP address 192.168.100.102 and callsign MYCALL-9. The bridge RPi will need to be connected by WiFi to the internet. 

## Additional setup instructions for Virtual Raspberry PIs

Instructions marked with a (\*) are only necessary for the virtual raspberry pi setup (but won't hurt the hardware-based setup). The internet on the virtual raspberry pi will come from the host computer (which may be connected to WiFi or ethernet cable) but will appear as ethernet (hardwired) to the virtual machine. Each raspberry pi should be configured (before boot up) with a second network interface of type "Internal" (the first network interface will be of type NAT). 

## Bridge Setup

1. Login to the RPi and/or bring up a terminal, in the `pi` user home directory
```
% cd ~
```
2. Install the necessary software to the RPi
```
% sudo apt-get install -y git ntp stunnel4 ax25-tools
```
3. Install the necessary software to the virtual RPi (\*)
```
% sudo apt-get install -y socat
```
4. Download all the repository files
```
% git clone https://github.com/silver-sat/arduinotnc.git
```
5. Add the contents of the file [arduinotnc/place_at_end_of_etc_rc.local](place_at_end_of_etc_rc.local) at the end of `/etc/rc.local` (replacing the existing "exit 0")
```
% sudo sed -e '$r /dev/stdin' -e '/exit 0/d' -i /etc/rc.local < arduinotnc/place_at_end_of_etc_rc.local
```
6. Link the [arduinotnc/bridge_startup_ax25.sh](bridge_startup_ax25.sh) file to `.startup.sh`
```
% ln -s arduinotnc/bridge_startup_ax25.sh .startup.sh
```
7. Append the [arduinotnc/bridge_axports](bridge_axports) file to `/etc/ax25/axports`
```
% sudo sed -i -e '$r arduinotnc/bridge_axports' /etc/ax25/axports
```
8. Link the [arduinotnc/rotatemap.conf](rotatemap.conf) file to `.rotatemap.conf`
```
% sudo ln -s arduinotnc/rotatemap.conf .rotatemap.conf
```
9. Link the [arduinotnc/stunnel.conf](stunnel.conf) file to `.stunnel.conf`
```
% ln -s arduinotnc/stunnel.conf .stunnel.conf
```
10. Link the [arduinotnc/slowsocat.sh](slowsocat.sh) file to `.slowsocat.sh` (\*)
```
% ln -s arduinotnc/slowsocat.sh .slowsocat.sh
```
11. Link the [arduinotnc/throttle.py](throttle.py) file to `.throttle.py` (\*)
```
% ln -s arduinotnc/throttle.py .throttle.py
```
12. Reboot and re-login
```
% sudo reboot
```
13. Watch the AX25 packets go by!
```
% cd ~
% tail -f .ax0.log
```

## Satellite Setup

1. Login to the RPi and/or bring up a terminal, in the `pi` user home directory
```
% cd ~
```
2. Install the necessary software to the RPi
```
% sudo apt-get install -y git ax25-tools ntpdate
% sudo apt-get remove ntp
```
3. Install the necessary software to the virtual RPi (\*)
```
% sudo apt-get install -y socat
```
3. Add the contents of the file [place_at_end_of_etc_rc.local](place_at_end_of_etc_rc.local) at the end of `/etc/rc.local`
4. Download all the repository files
```
% git clone https://github.com/silver-sat/arduinotnc.git
```
5. Copy the [satellite_startup_ax25.sh](satellite_startup_ax25.sh) file to `satellite_startup_ax25.sh`
```
% cp arduinotnc/satellite_startup_ax25.sh .startup.sh
```
6. Append the [satellite_axports](satellite_axports) file to `/etc/ax25/axports`
```
% sudo cat arduinotnc/satellite_axports >> /etc/ax25/axports
```
7. Copy the Python files
```
% cp arduinotnc/*.py .
```
8. Rename the twitter credentials file and then edit to insert the necessary credentials.
```
% mv twittercred.empty.py twittercred.py
% nano twittercred.py
```
9. Copy the example image
```
% cp arduinotnc/overhead.jpg .
```
10. Reboot and re-login
```
% sudo reboot
```
11. Tweet over the ax25 interface via the HTTPS proxy on the bridge RPi
```
% cd ~
% python3 tweetpic.py overhead.jpg
```
