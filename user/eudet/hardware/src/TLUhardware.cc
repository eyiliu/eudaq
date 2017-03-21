#include "TLUhardware.hh"
#include <iostream>
#include <ostream>
#include <vector>
#include <string>
#include "uhal/uhal.hpp"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// DAC

  void AD5665R::testme(){
    std::cout << "TEST TEST TEST" << std::endl;
  }

  AD5665R::AD5665R(){

  }

  void AD5665R::SetI2CPar( i2cCore *mycore, char addr ) {
    m_DACcore = mycore;
    m_i2cAddr= addr;
  }

  void AD5665R::SetIntRef(bool intRef, bool verbose){
    // Configure the voltage reference for the DACs.
    // Normally we want to use the external one
    std::string stat;
    unsigned char chrsToSend[2];
    chrsToSend[0]= 0x00;

    if (intRef) {
      stat= "INTERNAL";
      chrsToSend[1]= 0x01;
      //cmdDAC= [0x38,0x00,0x01]
    }
    else{
      stat= "EXTERNAL";
      chrsToSend[1]= 0x00;
      //cmdDAC= [0x38,0x00,0x00]
    }
    m_DACcore->WriteI2CCharArray(m_i2cAddr, 0x38, chrsToSend, 2);
    if (verbose)
      std::cout << "  DAC reference set to " << stat << std::endl;
  }

  void AD5665R::SetDACValue(unsigned char channel, uint32_t value) {
    unsigned char chrsToSend[2];

    std::cout << "  Setting DAC channel " << (unsigned int)channel << " = " << value << std::endl;

    if (( (unsigned int)channel < 0 ) || ( 7 < (unsigned int)channel )){
      std::cout << "\twriteDAC ERROR: channel " << int(channel)  << " not in range 0-7" << std::endl;
      return;
    }

    if (value<0){
      std::cout << "\twriteDAC ERROR: value" << value << "<0. Default to 0" << std::endl;
      value=0;
    }
    if (value>0xFFFF){
      std::cout << "\twriteDAC ERROR: value" << value << ">0xFFFF. Default to 0xFFFF" << std::endl;
      value=0xFFFF;
    }
    // set the value
    chrsToSend[1] = value&0xff;
    chrsToSend[0] = (value>>8)&0xff;
    m_DACcore->WriteI2CCharArray(m_i2cAddr, 0x18+(channel&0x7), chrsToSend, 2);//here
  }


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// I/O EXPANDER
