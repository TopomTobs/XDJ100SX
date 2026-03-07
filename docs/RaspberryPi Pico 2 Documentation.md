

# Introduction
This Build documentation should be a rebuild guide to enable everyone to rebuild the XDJ100sx. The Player build in this guide is a derivative from the original, it should work the same but changes are made internally. For example the micro-controller used in the original, is now [difficult to get](https://www.pjrc.com/store/teensylc.html), was replaced with the RaspberryPi Pico 2. An important addition to the original is the proper DAC used. 


## What do you need to build this one
#### Materials
+ CDJ100s
+ RaspberryPi 3b+
+ RaspberryPi Pico 2
+ DAC Plus or Pro (depending on if you need balanced audio) or any other good DAC
+ 5 inch touch Display
+ Rotary Encoder (KY-040)
+ USB-Port (Female)
+ 5V Power-Supply
+ Some cables(USB 2.0, audio, simple one strand)
+ Soldering stuff
#### Appliances
+ Soldering Iron
+ Multimeter (useful for debugging)
+ 3D Printer

# Building
## Dissembling XDJ100s
We only need the Housing and the PCB with the buttons and leds, optionally you could keep the power-supply PCB for the cinch output and on-off button.
You should be left with this:


## Button PCB
### Cutting traces and laying new Ground
We will have to cut some traces on the PCB first and then lay a new Ground. All input and output on this PCB share the same ground. The red lines are where you have to cut the PCB. The green marks indicate a bridge where for example a resistor was and the traces need to be connected (This is the case where led are). Check with a multimeter if every button and led is connected together via Ground. To Check if all Traces have been cut correctly one may use the debug code with Serial Monitor Output when a button is pressed (Arduino/Pico2/Debug just flash and check the Serial Monitor). 

![[NewGround.png]]

## Wiring Buttons to Pico 2


| Button         | Pin on Pico |
| -------------- | ----------- |
| eject          | 0           |
| track previous | 1           |
| track next     | 2           |
| search back    | 3           |
| search next    | 4           |
| cue            | 5           |
| jet            | 6           |
| play           | 7           |
| zip            | 8           |
| wah            | 9           |
| hold           | 10          |
| time           | 11          |
| mastertempo    | 12          |
| jog A          | 14          |
| jog B          | 15          |
| Browse A       | 16          |
| Browse B       | 17          |
| load           | 18          |
| led cue        | 19          |
| led Play       | 20          |
| led Intern     | 21          |
| led CD         | 22          |
| pitch          | A0          |


![[Wiring_Pico.png]]

### Wiring Jog
I use 10k Ohm Resistors as pullups. It should also be possible with Pico onboard Pullups but I am not quite sure with that (Improvements are to be made here).
![[Wiring_jog_Pico.png]]






# Hardware 





# Changes to the Linux envirement
The file in /home/xdj100sx/.mixxx/controllers/XDJ100SX.midi.xml have to be replaced with the Pico Midi mappings. (See mixxx/MIDI/RaspberryPi_Pico2 on Github) Also make sure you use the right file in the mixxx settings. (Options/Preferences/Controllers/NameofController Load Mapping: PICO2_MIDI_Mapping_File).
![[MIDI_Mapping.png]]
