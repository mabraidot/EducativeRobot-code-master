#include <Wire.h>
extern "C" {
  #include "utility/twi.h"
}

#define SLAVE_ACTIVATE_PIN 12
#define SLAVE_ADDRESS 0x01

byte blocks[255] = {0};
uint8_t status[3];
bool debug = true;

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
      delay(300);
      open_gate(block_address);
      delay(300);

      finding = true;
      block_position++;
      block_address++;
      
    }
  }
  scanResults();
}

void scanResults(){
  Serial.println(F("\nScanning I2C bus..."));
  for( byte i = 0; i < sizeof(blocks); i++ )
  {
    if(blocks[i])
    {
      Serial.print(i);
      Serial.print(F(": "));
      Serial.print(blocks[i]);
      Serial.print( (i%10) ? F("\t"):F("\n"));
    }
  }
  Serial.println(F("\nScanning finished\n"));

}

bool slaveExists(byte address){
  bool found = false;
  for( byte i = 0; i < sizeof(blocks); i++ ){
    if(blocks[i] == address){
      found = true;
    }
  }

  return found;
}




void flash_led(byte address)
{
  if(debug){
    if(!slaveExists(address)){
      Serial.print(address);
      Serial.println(F(": Doesn't exists."));
    }
  }
  Wire.beginTransmission(address);
  Wire.write(2);        // RegAddress
  Wire.write(7);        // Value
  Wire.endTransmission();
}

void open_gate(byte address)
{
  if(debug){
    if(!slaveExists(address)){
      Serial.print(address);
      Serial.println(F(": Doesn't exists."));
    }
  }
  Wire.beginTransmission(address);
  Wire.write(1);        // RegAddress
  Wire.write(1);        // Value
  Wire.endTransmission();
}

void close_gate(byte address)
{
  if(debug){
    if(!slaveExists(address)){
      Serial.print(address);
      Serial.println(F(": Doesn't exists."));
    }
  }
  Wire.beginTransmission(address);
  Wire.write(1);        // RegAddress
  Wire.write(0);        // Value
  Wire.endTransmission();
}

void add_slave(byte address)
{
  if(debug){
    if(!slaveExists(address)){
      Serial.print(address);
      Serial.println(F(": Doesn't exists."));
    }
  }
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(0);        // RegAddress
  Wire.write(address);  // Value
  Wire.endTransmission();
}


void read_status(byte address)
{
  Serial.println(F("\nSlave Status Start -----------------------------"));
  Serial.println(address, HEX);

  if(!slaveExists(address)){
    Serial.println(F("Doesn't exists."));
  }else{
  
    for(int j=0;j<sizeof(status);j++)
    {
      Wire.requestFrom(address, (uint8_t)1);
      if(Wire.available())
      {
        byte i = Wire.read();
        Serial.print(j);
        Serial.print(F(":\t"));
        Serial.print(i);
        Serial.print(F("\n"));
      }
    }

  }

  Serial.println(F("Slave Status End -------------------------------\n"));
}


uint8_t read_state(byte address, byte reg)
{
  if(debug){
    Serial.println(F("\nReg State Start -----------------------------"));
    if(!slaveExists(address)){
      Serial.print(address);
      Serial.println(F(": Doesn't exists."));
    }
  }

  for(int j=0;j<sizeof(status);j++)
  {
    Wire.requestFrom(address, (uint8_t)1);
    if(Wire.available())
    {
      status[j] = Wire.read();
    }
  }

  if(debug){
    Serial.print(reg);
    Serial.print(F("\t"));
    Serial.print(status[reg]);
    Serial.println(F("\nReg State End -------------------------------\n"));
  }

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

void help(){
  Serial.println(F("\nI2C CONSOLE INTERFACE"));
  Serial.println(F("Available commands:"));
  Serial.println(F(""));
  Serial.println(F("H: This help"));
  Serial.println(F("S: Scan for I2C slaves"));
  Serial.println(F("M: Show list of slaves found"));
  Serial.println(F("D: Disable all slaves"));
  Serial.println(F("L<addr>: Flash the onboard led of slave on <addr> address"));
  Serial.println(F("O<addr>: Activate the child slave of the slave on <addr> address"));
  Serial.println(F("C<addr>: Deactivate all the children slaves of the slave on <addr> address"));
}

void process_serial(){
  char cmd = Serial.read();
  if (cmd > 'Z') cmd -= 32;
  switch (cmd) {
    case 'H': help(); break;
    case 'S': scanI2CDevices(); break;
    case 'M': scanResults(); break;
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