#include <Wire.h>
extern "C" {
  #include "utility/twi.h"
}
char action;

void scanI2CDevices(void(*callback)(byte address, byte result) )
{
  Serial.println("\nScanning I2C bus...");
  byte rc;
  byte data = 0;
  for( byte addr = 1; addr <= 127; addr++ ) {
    rc = twi_writeTo(addr, &data, 0, 1, 0);
    callback( addr, rc );
  }
  Serial.println("\nScanning finished\n");
}
void scanResults( byte addr, byte result ) {
  Serial.print("Address: ");
  Serial.print(addr,HEX);
  Serial.print( (result==0) ? " Found!":"      ");
  Serial.print( (addr%4) ? "\t":"\n");
}

void process_serial(){
  char cmd = Serial.read();
  if (cmd > 'Z') cmd -= 32;
  action = cmd;
  switch (cmd) {
    case 'S': scanI2CDevices(scanResults); break;
    case 'L': flash_led(); break;
    case 'O': open_gate(); break;
    case 'C': close_gate(); break;
    case 'R': read_status(); break;
  }
  
  while (Serial.read() != 10); // dump extra characters till LF is seen (you can use CRLF or just LF)

}




void flash_led()
{
  Wire.beginTransmission(0x2F);
  Wire.write(1); // RegAddress
  Wire.write(4); // Value
  Wire.endTransmission();
}
void open_gate()
{
  Wire.beginTransmission(0x2F);
  Wire.write(0); // RegAddress
  Wire.write(1); // Value
  Wire.endTransmission();
}
void close_gate()
{
  Wire.beginTransmission(0x2F);
  Wire.write(0); // RegAddress
  Wire.write(0); // Value
  Wire.endTransmission();
}
void read_status()
{
  Serial.println("\nStart -----------------------------");
  for(int j=0;j<2;j++)
  {
    Wire.requestFrom(0x2F, 1);
    while(Wire.available())
    {
      int i = Wire.read();
      Serial.println(i);
    }
  }
  Serial.println("End -------------------------------\n");
}


void setup()
{
  Wire.begin();
  Serial.begin(9600);

  // wait for slave to finish any init sequence
  delay(2000);

  // scan for I2C devices connected
  scanI2CDevices( scanResults );
  
}



void loop()
{

  if (Serial.available()) process_serial();

}



