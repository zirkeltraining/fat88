// fat88
// addressing mode functions & constants

// the letters that can be displayed:
String ciphers="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-_ =";  

// this array stores the bit map for each letter that can be displayed.Since we have 7 segments, there are 7 bits for each cipher
// numbered in reading direction, MSB is on top and LSB on bottom.
char ciphSegs[]={B1110111,B0010010,B1011101,B1011011,B0111010,B1101011,B1101111,B1010010,B1111111,B1111011, 
                 B1111110,B0101111,B1100101,B0011111,B1101101,B1101100,B1111011,B0101110,B0100100,B1010111,B0111110,B0100101,B1110000,B0001110,B0001111,B1111100,B1111010,B0001100,B1101011,B0101101,B0110111,B0000111,B0000000,B0000000,B0111011,B1011101,
                 B0001000,B0000001,B0000000,B1000000};
                 
// this array stores a map of LED IDs for each segment that needs to be controlled. 
// there can be 3 LEDs for each segment. Since cabling may be arbitrary, there is a separate mapping for the left and right digit
byte segMap[2][7][3] = {
                      {   {0,1,2},{20,19,18} ,{3,4,5} ,{17,16,15} ,{14,13,12} ,{6,7,8} ,{11,10,9} },
                      {  {36,37,38},{35,34,33} ,{39,40,41} ,{29,28,27} ,{32,31,30} ,{26,25,24} ,{21,22,23} }
};

byte cycler=0;

void set7Seg(char ciphL,char ciphR) {
      
    byte c=ciphers.indexOf(ciphL);
    if (c==-1) {
      return;
    }
    // step through the seven segments and look up the three LED-IDs for each
    //Serial.println(ciphSegs[c],BIN);
  
    for (byte i=0;i<7;i++){
      boolean b=(ciphSegs[c] >> (6-i)) & B0000001;
      for (byte j=0; j<3;j++) {
          
        if (!b){
            /* #ifdef DEBUG
            Serial.print( segMap[side][i][j]);
            Serial.println ("-OFF");
            #endif
            */
            setLED(segMap[0][i][j],0x0);
          } else {
            /*#ifdef DEBUG
            Serial.print( segMap[side][i][j]);
            Serial.println ("-ON");
            #endif  */
            setLED(segMap[0][i][j],0xffffff);
        }
      
      }
    }
    
    c=ciphers.indexOf(ciphR);
    if (c==-1) {
      return;
    }
  
    for (byte i=0;i<7;i++){
      boolean b=(ciphSegs[c] >> (6-i)) & B0000001;
      for (byte j=0; j<3;j++) {
          
        if (!b){
            /* #ifdef DEBUG
            Serial.print( segMap[side][i][j]);
            Serial.println ("-OFF");
            #endif
            */
            setLED(segMap[1][i][j],0x0);
          } else {
            /*#ifdef DEBUG
            Serial.print( segMap[side][i][j]);
            Serial.println ("-ON");
            #endif  */
            setLED(segMap[1][i][j],0xffffff);
        }
      
      }
    }

  strip.show();
}

void setLED(int i, uint32_t col) {
  // at the moment, only direct mode. 
  // Todo: Add LERP with a defined fadetime.
  cycler++; 
  if (col==0) {
        strip.setPixelColor(i,col);
  } else {
    strip.setPixelColor(i,col);
  }
  
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


