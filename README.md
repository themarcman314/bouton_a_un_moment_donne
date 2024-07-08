This code is not even in alpha.

Was trying to figure out a version name so I asked chatgpt :

![cool image](https://github.com/themarcman314/bouton_a_un_moment_donne/blob/master/chatgpt_github.png?raw=true)

This code is in "proto".
I know it might sound similar to some but dont confuse this with "production".

## Usage

### Wire I2S pins from ESP8266NodeMCU to MAX98357A
Kindly RTFM and follow wiring from here :
https://github.com/earlephilhower/ESP8266Audio?tab=readme-ov-file#i2s-dacs

### Flash firmware
Open `.ino` file with the arduino ide and click the arrow pointing right to flash.

### Configure audio files
1. Connect to the WPA with Wi-Fi enabled device.
SSID name is `fbi_van`

2. Portal with the configuration page should open automatically.
If not just go to `192.168.1.1` or any ip at all really.

3. Upload small mp3 files (900 kB or smaller works well)

## TODO :
- Add more CSS for ugly buttons
- Make page adjust dynamically (for phones)
- Disable I2S pins after playing sound
- Fix overall bugs
- Finish hardware prototype
