#include <Wire.h>
extern "C" {
  #include "utility/twi.h"
}
#include "config.h"
#include "debug.h"

Debug debug;


char buffer[5];

byte blocks[255] = {0};
uint8_t status[4];

void scanI2CDevices()
{

  disable_slaves();
  enable_slaves();

  bool finding = true;
  bool finding_modifier = true;
  uint8_t discovered = 1;
  byte rc;
  byte data = 0;
  memset(blocks,0,sizeof(blocks));
  byte block_address = SLAVE_START_ADDRESS;
  byte block_position = 0;
  while(finding){
    // @TODO: make it recursive
    finding_modifier = true;
    while(finding_modifier){
      finding_modifier = false;
      rc = twi_writeTo(SLAVE_MODIFIER_ADDRESS, &data, 0, 1, 0);
      if(rc == 0){       // Block found
        //give it an address
        strcpy(buffer, (char*)block_address);
        debug.println(buffer);
        debug.println(F("Adding slave modifier ..."));
        add_slave(SLAVE_MODIFIER_ADDRESS, block_address);
        delay(600);
        // If modifier was activated successfully, open its gate
        if(read_state(block_address, 3)){
          blocks[block_position] = block_address;
          debug.println(F("Opening gate modifier ..."));
          open_gate(block_address);
          delay(300);
        
          finding_modifier = true;
          block_position++;
          block_address++;
        }
      }
    }

    finding = false;
    rc = twi_writeTo(SLAVE_ADDRESS, &data, 0, 1, 0);
    if(rc == 0){       // Block found
      //give it an address
      strcpy(buffer, (char*)block_address);
      debug.println(buffer);
      debug.println(F("Adding slave ..."));

      add_slave(SLAVE_ADDRESS, block_address);
      delay(600);
      // If slave was activated successfully, open its gate
      if(read_state(block_address, 3)){
        blocks[block_position] = block_address;
        debug.println(F("Opening gate ..."));
        open_gate(block_address);
        delay(300);

        finding = true;
        block_position++;
        block_address++;
      }
    }
  }


  scanResults();
}

void scanResults(){
  debug.println(F("\nScanning I2C bus..."));
  for( byte i = 0; i < sizeof(blocks); i++ )
  {
    if(blocks[i])
    {
      strcpy(buffer, (char*)i);
      debug.print(buffer);
      debug.print(F(": "));
      strcpy(buffer, (char*)blocks[i]);
      debug.print(buffer);
      debug.print( (i==0 || i%10) ? F("\t"):F("\n"));
    }
  }
  debug.println(F("\nScanning finished\n"));

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
  
  if(!slaveExists(address)){
    strcpy(buffer, (char*)address);
    debug.println(buffer);
    debug.println(F(": Doesn't exists."));
  }else{
    Wire.beginTransmission(address);
    Wire.write(2);        // RegAddress
    Wire.write(7);        // Value
    Wire.endTransmission();
  }
}

void open_gate(byte address)
{
  if(!slaveExists(address)){
    strcpy(buffer, (char*)address);
    debug.print(buffer);
    debug.println(F(": Doesn't exists."));
  }else{
    Wire.beginTransmission(address);
    Wire.write(1);        // RegAddress
    Wire.write(1);        // Value
    Wire.endTransmission();
  }
}

void close_gate(byte address)
{
  if(!slaveExists(address)){
    strcpy(buffer, (char*)address);
    debug.print(buffer);
    debug.println(F(": Doesn't exists."));
  }else{
    Wire.beginTransmission(address);
    Wire.write(1);        // RegAddress
    Wire.write(0);        // Value
    Wire.endTransmission();
  }
}

void add_slave(const byte old_address, byte address)
{
  /*if(!slaveExists(address)){
      strcpy(buffer, (char*)address);
      debug.print(buffer);
      debug.println(F(": Doesn't exists."));
  }else{*/
    Wire.beginTransmission(old_address);
    Wire.write(0);        // RegAddress
    Wire.write(address);  // Value
    Wire.endTransmission();
  //}
}


void read_status(byte address)
{
  debug.println(F("\nSlave Status Start -----------------------------"));
  strcpy(buffer, (char*)address);
  debug.println(buffer);

  if(!slaveExists(address)){
    debug.println(F("Doesn't exists."));
  }else{
  
    memset(status,0,sizeof(status));
    for(int j=0;j<sizeof(status);j++)
    {
      Wire.requestFrom(address, (uint8_t)1);
      if(Wire.available())
      {
        byte i = Wire.read();
        strcpy(buffer, (char*)j);
        debug.print(buffer);
        debug.print(F(":\t"));
        strcpy(buffer, (char*)i);
        debug.print(buffer);
        debug.print(F("\n"));
      }
    }

  }

  debug.println(F("Slave Status End -------------------------------\n"));
}


uint8_t read_state(byte address, byte reg)
{
  memset(status,0,sizeof(status));
  for(int j=0;j<sizeof(status);j++)
  {
    Wire.requestFrom(address, (uint8_t)1);
    if(Wire.available())
    {
      status[j] = Wire.read();
    }
  }

  debug.println(F("\nReg State Start -----------------------------"));
  strcpy(buffer, (char*)reg);
  debug.print(buffer);
  debug.print(F("\t"));
  strcpy(buffer, (char*)status[reg]);
  debug.print(buffer);
  debug.println(F("\nReg State End -------------------------------\n"));
  
  return status[reg];
}


void enable_slaves(){
  digitalWrite(SLAVE_ACTIVATE_PIN, HIGH);
  delay(300);
}

void disable_slaves(){
  digitalWrite(SLAVE_ACTIVATE_PIN, LOW);
  memset(blocks,0,sizeof(blocks));
  delay(300);
}

void help(){
  debug.println(F("\nI2C CONSOLE INTERFACE"));
  debug.println(F("Available commands:"));
  debug.println(F(""));
  debug.println(F("H: This help"));
  debug.println(F("S: Scan for I2C slaves"));
  debug.println(F("M: Show list of slaves found"));
  debug.println(F("D: Disable all slaves"));
  debug.println(F("R<addr>: Read the buffer status of slave on <addr> address"));
  debug.println(F("L<addr>: Flash the onboard led of slave on <addr> address"));
  debug.println(F("O<addr>: Activate the child slave of the slave on <addr> address"));
  debug.println(F("C<addr>: Deactivate all the children slaves of the slave on <addr> address"));
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