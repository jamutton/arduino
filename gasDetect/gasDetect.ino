/**
 * gasDetect
 * Uses an Adafruit-3199 (MiCS-5524 Gas Detector).  Every CYCLETIME ms it will bring
 * low the power-on pin for the gas sensor, wait HEATERWAITTIME ms for the heater to ready,
 * then collect MEASURETIME ms of samples, every MEASUREINTERVALTVL ms and output it to the
 * serial port for graphing
 * 
 * @TODO: add wifi, Zwave, or Xbee and send average as a mosquito event.
 * @TODO: add oled output of average.
 * 
 * @PINSETUP: Arduino -> 5524
 * 5v -> 5v
 * gnd -> gnd
 * A0 -> A0
 * D7 -> En
 * 
 * author: James Mutton
 * license: MIT
 */

#define CYCLETIME 10000       // Milliseconds to wait in between sample periods
#define SAMPLEPERIOD 10000    // Milliseconds length of sample period
#define MEASUREINTERVAL 100   // Milliseconds between samples during sample period
#define HEATERWAITTIME 2000   // Milliseconds to wait for the heater to ready
#define HEATERPIN 7           // Pin used for heater control

void setup() {
  Serial.begin(115200);
  pinMode(HEATERPIN, OUTPUT);
  digitalWrite(HEATERPIN, HIGH);
}
 
void loop() {
  int i;
  
  //pull pin 7 high to activate the gas-sensor's heater
  digitalWrite(HEATERPIN, LOW);
  delay(HEATERWAITTIME);
  for (i = 0; i < SAMPLEPERIOD/MEASUREINTERVAL; i++) {
    int reading = analogRead(A0);
    Serial.println(reading);
    delay(MEASUREINTERVAL);
  }
  digitalWrite(HEATERPIN, HIGH);
  // POSSIBLY TAKE AVERAGE AND EMIT TO OLED
  delay(CYCLETIME);
}
