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
  - add this to "Additional board manager URLs" in Arduino settings:
  - `https://www.pjrc.com/teensy/package_teensy_index.json`
- a Teensy 3.6
- a RaspperyPi Zero W (for over the air updates)


## Setup

Get the `teensy_loader_cli` available in the repo `firmware` directory:
```bash
cd firmware
brew install teensy_loader_cli
ln -s $(which teensy_loader_cli) ./teensy_loader_cli 
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



# Ideas for effects

* animation going for inside to the outside
* circles from mid to edge rotating at different speeds with diff
* spirals controllable with knobs and buttons
* classic lolly spirals red and white with one arm or many
* Bitmap/GIF rendering! For a smily or a heart
* a pulsing heart
* color sparkels
* fire
* plasma
* bouncy balls
* rains
* audio wave form visualisation
* little particles going in a direction that start with very white light and fade after
