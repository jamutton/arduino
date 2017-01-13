/**
 * tempMonitor
 * Uses an OSEPP LM-35 based temperature breakout board that I picked up from Frys.  This has 3
 * pins, VCC, GND and Signal.  It will sleep for CYCLETIME ms, wake up and measure for MEASURETIME
 * seconds then average and emit the output.  Not sure yet, what the encoding of the output
 * means, and it would be better to display it to an OLED or something but this is my starter.
 * 
 * @TODO: add wifi, Zwave, or Xbee and send average as a mosquito event.
 * @TODO: add oled output of average temp
 * 
 * @PINSETUP: Arduino -> OSEPP
 * 5v -> +
 * gnd -> -
 * A0 -> S
 * 
 * author: James Mutton
 * license: MIT
 */

#define CYCLETIME 300000       // how long to wait in between measurement cycles (in ms)
#define MEASURETIME 5000      // how long to measure for when generating an average (in ms)
#define MEASUREINTERVAL 100   // ms between measurements while taking them.

const int asize = MEASURETIME/MEASUREINTERVAL;

void setup() {
  Serial.begin(115200);
}

float averageOf(int a[], int sz) {
  int i, acc = 0;
  for (i = 0; i < sz; i++) {
    acc = acc + a[i];
  }
  if ( acc == 0 || sz == 0) return 0; 
  return (float)acc/(float)sz;
}

void loop() {
  int i;
  int samples[asize];
  Serial.println("starting measurement...");
  for (i = 0; i < asize; i++) {
    int reading = analogRead(A0);
    samples[i] = reading;
    delay(MEASUREINTERVAL);
  }
  Serial.print("Average temp: ");
  Serial.println(averageOf(samples, asize));
  delay(CYCLETIME);
}
