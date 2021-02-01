# MAX186_Rescue
Arduino project to convert a printer port connected MAX186 ADC to USB serial.


The MAX186 IC showed up in another printer port connected circuit in KIT118 from kitsrus.com. It appears to still be available from them. 
For sometime I have thought about writing a program for the Arduino which would interface with the MAX186 IC pins and allow ADC data to flow
through the Arduino USB serial port instead of the printer port.  The issue recently came up again so this time I rummaged around and found 
a MAX186 IC and an  Arduino Uno.  A few resistors  and 2 capacitors later I had the circuit breadboarded. There is no reason why a Arduino Nano
couldn't be used instead of the Uno board. 

As written, the code has some limitations.  It always transmits all eight channels of the MAX186 ADC.  All that is needed to change to a different number of channels is to change the code below in the GetD() function so that the '8' in the for statement is the number of channels you want.

void GETD(){

         int u;

         for(u=1 ; u<=8; ++u)

         {

            dat = MaxRead(u-1);

         }

         Serial.print("^^3001"); // This tells RSP to time stamp it

         Serial.write(255); // all commands end with this character.

         return;

  }

Follow the pin connections shown in the JPG. 

See more documentation at: https://cygnusa.blogspot.com/2021/02/rescue-that-printer-port-adc.html
