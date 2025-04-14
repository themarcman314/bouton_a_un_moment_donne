## HW prototype
kicad schematic and board files as well as BOM and step files are available here.
![cool image](https://github.com/themarcman314/bouton_a_un_moment_donne/blob/master/pcb.png?raw=true)

This code is not even in alpha.

Was trying to figure out a version name so I asked chatgpt :

![cool image](https://github.com/themarcman314/bouton_a_un_moment_donne/blob/master/chatgpt_github.png?raw=true)

This code is in "proto".

I know it might sound similar to some but dont confuse this with "production".

## Usage

### Clone the repo
HTTPS
```bash
$ git clone "https://github.com/themarcman314/bouton_a_un_moment_donne.git"
```
SSH
```bash
$ git clone git@github.com:themarcman314/bouton_a_un_moment_donne.git
```

### Wire I2S pins from ESP8266NodeMCU to MAX98357A DAC & class D amp
Kindly RTFM and follow wiring from here :
https://github.com/earlephilhower/ESP8266Audio?tab=readme-ov-file#i2s-dacs

### Flash firmware
Open `.ino` file with the arduino ide and click the arrow pointing right to flash.

### Configure audio files
1. Connect to the WAP with Wi-Fi enabled device.
SSID name is `fbi_van`

2. Portal with the configuration page should open automatically.
If not just go to `192.168.1.1` or any ip at all really.

3. Upload small mp3 files (2 MByte or smaller works well). My esp8266 module is limited to 4MB, I am allocating 3MB to the filesystem. Even when doing so, the amount of files that can be uploaded is limited depending on the bit rate, duration of audio, etc...
Remaining space is displayed on the main page.

Be patient when uploading a file.
Uploading a 1.1 MByte file took me 15 seconds which gives a transfering speed of about 1.1 * 8 / 15 = 0.58 MBit/s

Below is the configuration I set in arduino-ide to increase the upload speed

![cool image](https://github.com/themarcman314/bouton_a_un_moment_donne/blob/master/upload_speed_optimisation_parameters.png?raw=true)
