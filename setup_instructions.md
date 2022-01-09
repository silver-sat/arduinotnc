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
% sudo apt-get install -y git ntp stunnel4 ax25-tools ax25-apps
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
8. Fix permissions of file [arduinotnc/rotatemap.conf](rotatemap.conf) and link to `.rotatemap.conf`
```
% sudo chown root.root arduinotnc/rotatemap.conf
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
12. Allow the satellite raspberry pi to access the current time from the NTP server
```
% echo "restrict 192.168.100.0 mask 255.255.255.0" | sudo sed -e '/#restrict 192.168.123.0/r /dev/stdin' -i /etc/ntp.conf
```
13. Write down the internal network interface IP address (second line, after `inet`). Should be of the form `169.254.???.???`. (\*)
```
% ifconfig eth1
```
14. Reboot and re-login
```
% sudo reboot
```
15. Watch the AX25 packets go by!
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
% sudo apt-get install -y git ax25-tools ax25-apps ntpdate 
% sudo apt-get remove -y ntp
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
6. Link the [arduinotnc/satellite_startup_ax25.sh](satellite_startup_ax25.sh) file to `.startup.sh`
```
% ln -s arduinotnc/satellite_startup_ax25.sh .startup.sh
```
7. Append the [arduinotnc/satellite_axports](satellite_axports) file to `/etc/ax25/axports`
```
% sudo sed -i -e '$r arduinotnc/satellite_axports' /etc/ax25/axports
```
8. Fix permissions of file [arduinotnc/rotatemap.conf](rotatemap.conf) and link to `.rotatemap.conf`
```
% sudo chown root.root arduinotnc/rotatemap.conf
% sudo ln -s arduinotnc/rotatemap.conf .rotatemap.conf
```
9. Create the twitter credentials file:
```
% cp arduinotnc/twittercred.empty.py arduinotnc/twittercred.py 
```
10. Edit arduinotnc/twittercred.py to add Twitter credentials.
```
% nano arduinotnc/twittercred.py
```
11. Link the Python files
```
% ln -s arduinotnc/tweetpic.py 
% ln -s arduinotnc/twitterproxy.py
% ln -s arduinotnc/twittercred.py
```
9. Link the example image
```
% ln -s arduinotnc/overhead.jpg
```
10. Link the payload driver script
```
% ln -s arduinotnc/payload.sh
```
11. Fix the bridge internal network interface IP address. Edit `.startup.sh`, find `BRIDGE_ETH1_IP=` and change the IP address to that from step 13 of the bridge setup. (\*).
```
% nano .startup.sh
```
12. Reboot and re-login
```
% sudo reboot
```
13. Tweet over the ax25 interface via the HTTPS proxy on the bridge RPi
```
% cd ~
% ./payload.sh
```
