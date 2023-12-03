# another2048 for the gamebuino classic
## Description
This is another 2048 game for the gamebuino classic. I created this myself because the original from the wiki does not work properly for me.
## Setup
Copy the build/arduino.avr.uno/another2048.ino.hex onto your SD-card (not tested jet because my DIY gambuino has no SD-card).
Alternatively open the another2048.ino in your Arduino IDE and upload it directly.
## Usage
Just control it with the D-pad.
If you play beyond 8192 the message "Victory!" appears to prevent the numbers from overflowing the cells.
Note that at the moment there is no game-over screen. If you can't move anymore then you have lost. I'm going to change it soon. Restart by pressing B.
