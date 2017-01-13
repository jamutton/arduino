/**
 * IRSender.  Sketch for remotely controlling a Kinivo 501BN HDMI Switch over infrared.
 */
#include <IRLib.h>
 
//IRsend irSender;
IRsendRaw irSender;

// uses these pins going high to indicate a request to send.
const int NUMBER_PADS[4]  = {11,10,9,8};
// Kinivo 501BN Input 1 mark/space timings
unsigned int rawbuf1[] = {8900, 4650, 400, 750, 400, 1800, 400, 750, 400, 750, 350, 750, 400, 750, 400, 700, 400, 750, 400, 1850, 400, 700, 400, 1850, 400, 1850, 400, 1800, 400, 1850, 400, 1850, 400, 1800, 400, 750, 400, 1800, 400, 1850, 400, 750, 400, 700, 400, 750, 400, 750, 400, 700, 400, 1850, 400, 750, 350, 750, 400, 1800, 450, 1800, 400, 1850, 400, 1850, 400, 1800, 400};
// Kinivo 501BN Input 2 mark/space timings
unsigned int rawbuf2[] = {8900, 4600, 400, 800, 350, 1850, 400, 750, 350, 800, 350, 750, 400, 750, 350, 750, 400, 750, 350, 1850, 400, 750, 400, 1850, 350, 1850, 400, 1850, 400, 1850, 350, 1850, 450, 1800, 400, 750, 350, 1850, 400, 750, 400, 1800, 400, 800, 350, 750, 400, 750, 350, 750, 400, 1800, 450, 750, 350, 1850, 400, 750, 350, 1850, 400, 1850, 400, 1800, 400, 1900, 350};
// Kinivo 501BN Input 3 mark/space timings
unsigned int rawbuf3[] = {8900, 4650, 400, 750, 350, 1850, 400, 750, 400, 700, 400, 800, 350, 750, 400, 750, 350, 800, 350, 1850, 400, 750, 350, 1850, 400, 1850, 400, 1800, 450, 1800, 400, 1850, 400, 1850, 400, 750, 350, 1850, 400, 1800, 450, 1800, 450, 700, 400, 800, 350, 700, 400, 750, 400, 1800, 400, 750, 400, 750, 400, 750, 350, 1850, 400, 1850, 400, 1800, 400, 1850, 400};
// Kinivo 501BN Input 4 mark/space timings
unsigned int rawbuf4[] = {8850, 4650, 400, 750, 400, 1800, 400, 750, 400, 750, 400, 750, 350, 750, 400, 750, 350, 750, 400, 1850, 400, 750, 350, 1850, 400, 1850, 400, 1800, 450, 1850, 350, 1800, 450, 1800, 450, 1800, 400, 750, 400, 750, 350, 1850, 400, 850, 300, 750, 350, 800, 350, 750, 400, 700, 400, 1800, 450, 1800, 400, 750, 400, 1800, 450, 1800, 400, 1850, 450, 1750, 450};
// Kinivo 501BN Input 5 mark/space timings
unsigned int rawbuf5[] = {8950, 4650, 400, 750, 400, 1850, 400, 700, 400, 800, 350, 750, 400, 700, 400, 750, 400, 750, 400, 1800, 400, 750, 400, 1800, 450, 1800, 400, 1850, 400, 1850, 400, 1850, 400, 1800, 400, 1850, 400, 750, 400, 1800, 400, 1900, 350, 750, 400, 750, 350, 750, 400, 750, 400, 700, 400, 1850, 400, 750, 400, 750, 350, 1850, 400, 1850, 400, 1800, 450, 1800, 400};
// Kinivo 501BN Up mark/space timings
unsigned int rawbufu[] = {8850, 4650, 400, 750, 400, 1850, 400, 700, 400, 750, 350, 800, 400, 700, 400, 750, 400, 700, 400, 1800, 450, 750, 400, 1800, 400, 1800, 450, 1800, 450, 1800, 400, 1800, 450, 1800, 400, 750, 400, 750, 350, 800, 350, 750, 400, 700, 400, 750, 400, 750, 400, 750, 350, 1850, 400, 1800, 400, 1850, 400, 1850, 400, 1850, 400, 1800, 400, 1800, 450, 1800, 450};
// Kinivo 501BN Down mark/space timings
unsigned int rawbufd[] = {8900, 4600, 400, 750, 400, 1800, 450, 700, 400, 750, 400, 750, 350, 750, 400, 750, 400, 700, 400, 1850, 400, 750, 400, 1800, 400, 1850, 400, 1800, 450, 1800, 400, 1850, 400, 1800, 450, 700, 400, 750, 400, 1850, 400, 700, 400, 750, 400, 750, 350, 750, 400, 750, 350, 1850, 400, 1850, 400, 750, 350, 1850, 400, 1850, 400, 1800, 400, 1850, 400, 1850, 400};
// Kinivo 501BN Menu mark/space timings
unsigned int rawbufm[] = {8900, 4650, 350, 750, 400, 1850, 400, 750, 350, 750, 400, 750, 400, 700, 400, 750, 400, 750, 400, 1800, 400, 750, 400, 1800, 450, 1800, 400, 1850, 400, 1800, 400, 1850, 400, 1850, 400, 700, 450, 1800, 400, 1850, 400, 700, 400, 1850, 400, 750, 350, 800, 350, 750, 400, 1800, 450, 700, 400, 750, 400, 1800, 400, 750, 400, 1850, 350, 1850, 400, 1850, 400};

// Number of mark+space in the timing
unsigned char rawlen = 68;

// Kilohertz of the timer frequency for Kinivo
// The frequency is the expected bandpass filter frequency at the receiver, where 38 is the most commonly used.  I tried 39 first and it worked so leaving as is
unsigned char rawhz = 39;

void setup()
{
  Serial.begin(9600);
  // initialize the pushbutton pins as inputs
  for (int thisPin = 0; thisPin < 4; thisPin++)  {
    pinMode(NUMBER_PADS[thisPin], INPUT);      
  }
}

void loop() {
  int buttonPress = 0;

  // Loop for a while, detecting any button presses
  while (buttonPress == 0) {
    if (digitalRead(NUMBER_PADS[0]) == HIGH) buttonPress = 1;
    if (digitalRead(NUMBER_PADS[1]) == HIGH) buttonPress = 2;
    if (digitalRead(NUMBER_PADS[2]) == HIGH) buttonPress = 3;
    if (digitalRead(NUMBER_PADS[3]) == HIGH) buttonPress = 4;
    delay(100);
  }

  // act on the button press
  switch (buttonPress) {
    //case 49: // for number 1 on serial input
    case 1:
      Serial.println("1 pressed");
      irSender.send(rawbuf1, rawlen, rawhz);
      break;
    //case 51:
    case 2:
      Serial.println("2 pressed");
      irSender.send(rawbuf3, rawlen, rawhz);
      break;
    //case 117:
    case 3:
      Serial.println("3 pressed");
      irSender.send(rawbufu, rawlen, rawhz);
      break;
    //case 100:
    case 4:
      Serial.println("4 pressed");
      irSender.send(rawbufd, rawlen, rawhz);
      break;
    default:
    break;
  }
  buttonPress = 0;
  delay(500);  // prevent duplicate code-entries
}
