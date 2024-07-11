# L O L L Y

⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⡶⠾⠿⠿⠷⠶⣦⣄⡀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠋⣁⣤⣶⣾⣿⣶⣦⣄⠙⠻⣦⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠟⢀⣾⣿⠟⠋⣉⣉⡉⠛⢿⣿⣄⠙⣷⡀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡟⢀⣿⡿⠁⣴⣿⠿⠿⣿⣶⡀⠻⣿⣇⢸⣇⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⢸⣿⡇⢸⣿⡇⠀⣦⠈⢿⣿⠀⢻⣿⠀⣿⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⠈⣿⣷⡀⠻⣿⣿⡿⢀⣾⣿⠀⣼⣿⠀⡏⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣆⠘⢿⣿⣦⣤⣠⣤⣾⡿⠋⣰⣿⡟⠘⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣄⠙⢷⣄⡉⠛⠿⠿⠛⠋⣠⣴⣿⠟⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⠟⠀⠀⠉⠛⠓⠶⠶⠶⠿⠟⠛⠁⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⢀⣴⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⣠⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⣠⣾⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⣼⠿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀


Lick it! Get electroCUTEd!

## Prerequisites

- [Arduino 2.3.2](https://www.arduino.cc/en/Main/Software?)
- `brew install arduino-cli` might be enough
- [latest Teensyduino](https://www.pjrc.com/teensy/td_download.html)
- a Teensy 3.6
- a RaspperyPi Zero W (for over the air updates)


## Setup

### Add the Teensy to know Arduino boards 
Add the following to your boards.txt (on OSX in /Applications/Arduino.app/Contents/Java/hardware/teensy/avr/boards.txt)

```
teensy36.build.fcpu=180000000
teensy36.build.usbtype=USB_SERIAL
teensy36.build.keylayout=US_ENGLISH
teensy36.build.flags.optimize=-Os
teensy36.build.flags.ldspecs=--specs=nano.specs
```

### Needed libraries

To make it easier - every lib including the Teensy core are in this repo :) 


**Important** for the potentiometer to work correctly, the Teensy Audio library needed to be edited. Open `input_adc.cpp` (to be found in the Teensy audio library folder) and find the call to `analogReference(INTERNAL);` and comment it out. (See https://github.com/PaulStoffregen/Audio/blob/master/input_adc.cpp#L51)

# Simulator

Check out: https://wokwi.com/projects/403018840445060097

# Remark

The code base is in no way idiomatic C/C++ (i.e. directly including files without headers) or written to please the eye. :-)

# Build

```
make
```

# Deploy

Over the air deployment relies on:
- the PI is connected to the local network
- the PI and the Teensy are correctly connected
- the teensyloader is available on the PI

```
make deploy
```

# Serial port on Pi

TODO



# List of modes we want to do
