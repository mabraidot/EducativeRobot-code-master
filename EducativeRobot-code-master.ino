#include <Wire.h>
extern "C" {
  #include "utility/twi.h"
}

#define SLAVE_ACTIVATE_PIN 12
#define SLAVE_ADDRESS 0x01

byte blocks[255] = {0};
uint8_t status[3];


void scanI2CDevices()
{

  disable_slaves();
  enable_slaves();

  bool finding = true;
  uint8_t discovered = 1;
  byte rc;
  byte data = 0;
  memset(blocks,0,sizeof(blocks));
  byte block_address = SLAVE_ADDRESS + 1;
  byte block_position = 0;
  while(finding)
  {
    finding = false;
    rc = twi_writeTo(SLAVE_ADDRESS, &data, 0, 1, 0);
    if(rc == 0)       // Block found
    {
      //give it an address
      blocks[block_position] = block_address;
      add_slave(block_address);
      delay(1000);
      open_gate(block_address);
      delay(500);

      finding = true;
      block_position++;
      block_address++;
      
    }
  }
  scanResults();
}

void scanResults(){
  Serial.println("\nScanning I2C bus...");
  for( byte i = 0; i < sizeof(blocks); i++ )
  {
    if(blocks[i])
    {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(blocks[i]);
      Serial.print( (i%10) ? "\t":"\n");
    }
  }
  Serial.println("\nScanning finished\n");

}





void flash_led(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(2); // RegAddress
  Wire.write(7); // Value
  Wire.endTransmission();
}

void open_gate(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(1); // RegAddress
  Wire.write(1); // Value
  Wire.endTransmission();
}

void close_gate(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(1); // RegAddress
  Wire.write(0); // Value
  Wire.endTransmission();
}

void add_slave(byte address)
{
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(0); // RegAddress
  Wire.write(address); // Value
  Wire.endTransmission();
}


void set_discovered(byte address)
{
  Wire.beginTransmission(address);
  Wire.write(0); // RegAddress
  Wire.write(1); // Value
  Wire.endTransmission();
}

void read_status(byte address)
{
  Serial.println("\nSlave Status Start -----------------------------");
  Serial.println(address, HEX);

  for(int j=0;j<sizeof(status);j++)
  {
    Wire.requestFrom(address, (uint8_t)1);
    if(Wire.available())
    {
      byte i = Wire.read();
      Serial.print(j);
      Serial.print(":\t");
      Serial.print(i);
      Serial.print("\n");
    }
  }
  Serial.println("Slave Status End -------------------------------\n");
}
uint8_t read_state(byte address, byte reg)
{
  //Serial.println("\nReg State Start -----------------------------");
  for(int j=0;j<sizeof(status);j++)
  {
    Wire.requestFrom(address, (uint8_t)1);
    if(Wire.available())
    {
      status[j] = Wire.read();
    }
  }
  
  /*Serial.print(reg);
  Serial.print("\t");
  Serial.print(status[reg]);
  Serial.println("\nReg State End -------------------------------\n");
  */
  return status[reg];
}


void enable_slaves(){
  digitalWrite(SLAVE_ACTIVATE_PIN, HIGH);
  delay(500);
}

void disable_slaves(){
  digitalWrite(SLAVE_ACTIVATE_PIN, LOW);
  memset(blocks,0,sizeof(blocks));
}


void process_serial(){
  char cmd = Serial.read();
  if (cmd > 'Z') cmd -= 32;
  switch (cmd) {
    case 'S': scanI2CDevices(); break;
    case 'L': {
        flash_led(Serial.parseInt()); 
      }
      break;
    case 'O': {
        open_gate(Serial.parseInt()); 
      }
      break;
    case 'C': {
        close_gate(Serial.parseInt()); 
      }
      break;
    case 'R': {
        read_status(Serial.parseInt()); 
      }
      break;
    case 'D': disable_slaves(); break;
  }
  
  while (Serial.read() != 10); // dump extra characters till LF is seen (you can use CRLF or just LF)

}






void setup()
{
  Wire.begin();
  Serial.begin(115200);

  pinMode(SLAVE_ACTIVATE_PIN, OUTPUT);             // Fist slave enable pin
  digitalWrite(SLAVE_ACTIVATE_PIN, LOW);

  // wait for slave to finish any init sequence
  delay(2000);

  // scan for I2C devices connected
  //scanI2CDevices( scanResults );
  
}

void loop()
{

  if (Serial.available()) process_serial();

}