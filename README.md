# Introduction-to-Robotics-2

                                                      ◄♠♣♦♥ABOUT♥♦♣♠►
*__Walrus__* is a learning device designed for children. 

Adults often complain that kids nowadays miss on lots of things because they lack enthusiasm and the curiosity to learn new things. 
In order to awake once again this passion in the new generation, I brought back an old concept: morse code and gave him a rather "flashy" appearance.

Using this device, one can both decode morse code and encode natural language. Walrus is a learning assistant, its main feature being the two LEDs on the head. They both guide beginners, showing them when they exceed the length of a dot and are now aiming for a dash. Moreover, you can delete the last character written by pressing a button.

                                                    ◄♠♣♦♥CONVENTIONS♥♦♣♠►
The dot duration is the basic unit of time measurement in Morse code transmission. 

The duration of a dash is three times the duration of a dot. 

Each dot or dash within a character is followed by period of signal absence, called a space, equal to the dot duration. 

The letters of a word are separated by a space of duration equal to three dots.

The words are separated by a space equal to seven dots.

                                                    ◄♠♣♦♥COMPONENTS♥♦♣♠►
→ Arduino UNO Board

→ 2 Buttons: 

- initially used to choose between the two modes (encode | decode) 
- later, in the "decode" mode one sends continuous wave and the other deletes written characters on the LCD

→ 2 LEDs: Blue and Red

→ LCD

→ Buzzer

→ Resistors

→ Wires

→ Breadboards

                                                       ◄♠♣♦♥DEMO♥♦♣♠►
I uploaded below a short video and a few pictures for a better understanding.

**YouTube:** https://www.youtube.com/watch?v=pn9ptvcVCXs&t=1s

**Drive:** https://drive.google.com/open?id=1-ESTeLk-QLDDwio-qJ3fmqzUwWonwTUS

                                                    ◄♠♣♦♥IMPROVEMENTS♥♦♣♠►
→ Adjust the morse code transmission rate to the speed of the current "player"
- in the beginning, he sends three V letters and then the algorithm adjusts the pauses 
→ Toggle between modes
→ Instead of pressing a button, simply knock
