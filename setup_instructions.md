# Setup instructions for Raspberry PIs

The two RPis will be called bridge, with IP address 192.168.100.101, and satellite, with IP address 192.168.100.102.

## Bridge Setup

1. Login to the RPi and/or bring up a terminal, in the `pi` user home directory
```
% cd ~
```
2. Install the necessary software to the RPi
```
% sudo apt-get install git ntp stunnel ax25-tools ntpdate
```
3. Add the contents of the file [place_at_end_of_etc_rc.local](place_at_end_of_etc_rc.local) at the end of `/etc/rc.local`
4. Download all the repository files
```
% git clone https://github.com/silver-sat/arduinotnc.git
```
5. Copy the [bridge_startup_ax25.sh](bridge_startup_ax25.sh) file to `.startup.sh`
```
% cp arduinotnc/bridge_startup_ax25.sh .startup.sh
```
6. Copy the [stunnel.conf](stunnel.conf) file to `stunnel.conf`
```
% cp arduinotnc/stunnel.conf stunnel.conf
```

## Satellite Setup

1. Login to the RPi and/or bring up a terminal, in the `pi` user home directory
```
% cd ~
```
2. Install the necessary software to the RPi
```
% sudo apt-get install git ntp stunnel ax25-tools ntpdate
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
6. Copy the Python files
```
% cp arduinotnc/*.py .
```
7. Rename the twitter credentials file and then edit to insert the necessary credentials.
```
% mv twittercred.empty.py twittercred.py
% nano twittercred.py
```
8. Copy the example image
```
% cp arduinotnc/*.jpg .
```
9. 
