# LightAlarm
An alarm clock but instead of sound 🔊, with light 🌞

Controlled with a website.

![Picture of the LightAlarm from above](/images/aboveworking.jpg "From above")

## Parts:
1. ESP8266 (Any will do)
2. AC-DC 12V adapter (Or replace 12V with the voltage of your choice)
3. DC-DC 12V-3.3V converter
4. 12V LED (in my case 2 connected parallel)
5. AO3400 MOSFET

## Schematics

![Picture of the schematic](/images/schematic.png "Schematic")

## Software
Written in c++ with [Platformio](https://platformio.org/)

Written using parts of Bootstrap.

![Picture of the website](/images/software_phone.png "From phone")

## Future plans:
 
- Normal lamp function
- Controlling buttons
- Google Assistant support