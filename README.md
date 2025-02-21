# Testing Commands
### Record 5-second test audio clip
`arecord -D hw:3,1 -c2 -r 48000 -f S16_LE -t wav -V stereo -v -d 5 file_stereo.wav`

This sets the hardware device as card 3 subdevice 1, 16 bit data, stereo, 48khz. The Rpi hardware interpolates samples so the rate is exactly 48kHz.

# I2S overlay
used overlay from https://github.com/AkiyukiOkayasu/RaspberryPi_I2S_Slave 

This configures the Pi as a listen only device where the clock is provided by the sending device. 
