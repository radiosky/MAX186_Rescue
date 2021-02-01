// Skeleton sketch for UDS connection to Radio-SkyPipe 
 // there are a lot of commented out print statements that
 // can be helpful in debugging.
//  This version can read from a Max186 ADC

#define MAX_SCLK 2 // max pin  19
#define MAX_CS 3 // max pin  18
#define MAX_DIN 4 // max pin  17
#define MAX_DOUT 5 // max pin  15


 int POLL; // if =1 then data is polled by RSP using a GETD command
 int STAT; // -1 = we were just stopped by a KILL command 0 = startup state 1 = INIT rcvd 2 = Ready to Go 3= Running
      int incomingByte;
      int Channels;
      byte CHX ;
 
 void setup() {
    pinMode(MAX_SCLK, OUTPUT);
    pinMode(MAX_CS, OUTPUT);
    pinMode(MAX_DIN, OUTPUT);
    pinMode(MAX_DOUT, INPUT);
    
     STAT = 0;
     POLL = 0;
     Serial.begin(19200); // connect to the serial port
     //delay(1000);
     //Serial.print("^^1002Arduino UDS");
     //Serial.write(255);
    }
 
 
 void loop() {
     // if we are pushing the data to RSP then we need to
     // establish our timing for sending new data.
     // here we are just doing a delay of 100ms to get a
     // sample rate of about 10 samples / sec.
     if (POLL == 0 && STAT ==3){
     delay(100); 
     } 

 while (Serial.available() > 0) {

     // read the oldest byte in the serial buffer:
     incomingByte = Serial.read();

     // if it's an K we stop (KILL):
     if (incomingByte == 'K') {
         //Serial.print("^^1002DEAD"); // Just for troubleshooting
         //Serial.write(255);
         //Serial.println("Arduino UDS");
         //GET PAST THE REST OF THE WORD by Reading it.
         delay(10); // not sure why these delays were needed
         incomingByte = Serial.read();
         delay(10);
         incomingByte = Serial.read();
         delay(10);
         incomingByte = Serial.read();
         incomingByte = 0;
         STAT=-1 ;
         }
         // if it's a capital I run the INIT code if any
         if (incomingByte == 'I' && STAT ==0) {
                 //INIT
                 // GET RID OF 'NIT'
                 delay(10);
                 incomingByte = Serial.read();
                 delay(10);
                 incomingByte = Serial.read();
                 delay(10);
                 incomingByte = Serial.read();
                 incomingByte = 0;
                 STAT = 1 ;
                 //Serial.print("^^1002 INITIALIZED "); 
                 //Serial.write(255);
         } 
         // if it's an L (ASCII 76) RSP will POLL for data
                 if (incomingByte == 'L') {
                 POLL = 1;
                 // GET RID OF 'L'
                 delay(10);
                 incomingByte = Serial.read();
                 incomingByte = 0;
                 //Serial.print("^^1002 POLLING ");
                 //Serial.write(255); 
         }

         // H sets it to push
         if (incomingByte == 'H') {
                 POLL = 0;
                // Serial.print("^^1002 PUSHING ");
                // Serial.write(255); 
         }

         // if it's a C then Radio-SkyPipe is requesting number of channels
         if (incomingByte == 'C') {
                 // change the last digit to = digit of channels of data (ex. 1)
                 delay(10);
                 Serial.print("^^20138");
                 Serial.write(255); // print result;
                 STAT = 2; // ready to go
         }
         if (incomingByte == 'A' ) {
                 // A means STAT was requested so send UDS ready message
                 delay(10);
                 Serial.print("^^1001");
                 Serial.write(255); // print result;;
                 // GET RID OF 'T'
                 delay(10);
                 incomingByte = Serial.read();
                 incomingByte = 0;
                 STAT=3;
            }
         
         // if it's an D we should send data to RSP:
         if (incomingByte == 'D' && POLL == 1 ) {
                 Serial.println(" DATA REQUEST RECEIVED ");
                 GETD() ;  
         }
         if (STAT== -1){
         STAT = 0;
         } 

     } 
     // we are finished processing any incoming commands from the PC
     // and we are not being polled so get a sample and send it
     if (POLL == 0 && STAT == 3) {
     GETD ();
     }
 }


 // This is where data is fetched and sent on to RSP.

 long int dat; //may have to change type to suit your data
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

 // clock commands into the MAX186
 void WRITE_HI()
    {
      digitalWrite (MAX_DIN, HIGH);
      digitalWrite (MAX_SCLK, LOW);
      //delay (1);
      digitalWrite (MAX_SCLK, HIGH);
    }

  void WRITE_LO()
    {
      digitalWrite (MAX_DIN, LOW);
      digitalWrite (MAX_SCLK, LOW);
      //delay (1);
      digitalWrite (MAX_SCLK, HIGH);
    }

long int MaxRead (int CH){
         long int OUT_WORD = 0;
         int ctr = 0;
        
        digitalWrite (MAX_CS, HIGH);
        digitalWrite (MAX_SCLK, LOW);
        digitalWrite (MAX_DIN, LOW);
        digitalWrite (MAX_CS, LOW);
        //CH = CH-1;
        WRITE_HI();
        // sel bit 2
        if (CH & 0x01){
            digitalWrite (MAX_SCLK, LOW);
            digitalWrite (MAX_SCLK, HIGH);}
          else
          {
            WRITE_LO();
          }
        
        // sel bit 1
        if (CH > 3)
            {
              WRITE_HI();
            }
        else
            {
              WRITE_LO();
            }     
        //sel bit 0
        if (CH & 0x02){
              WRITE_HI();
        }
        else
        {
          WRITE_LO();
        }
        
          // assume Unipolar - clk out a 
         WRITE_HI();
        // assume single ended 
         WRITE_HI();
        // internal clock mode
         WRITE_HI();
         WRITE_LO();
         WRITE_HI();
         digitalWrite (MAX_DIN, LOW);
         digitalWrite (MAX_SCLK, LOW);      
        
         for (ctr = 0; ctr < 12; ++ctr){
            OUT_WORD = OUT_WORD*2;
            digitalWrite (MAX_SCLK, HIGH);
            digitalWrite (MAX_SCLK, LOW);
            if (digitalRead (MAX_DOUT)== HIGH)
            {
              bitSet(OUT_WORD,0);  // = OUT_WORD || 0x01;
            }
           else
           {
            bitClear (OUT_WORD,0);
           }
         }
         Serial.print("#"); // # followed by channel number of data 
         Serial.print(CH);
         Serial.print(OUT_WORD); 
         Serial.write(255);
          
          
          return OUT_WORD;
         }


 
