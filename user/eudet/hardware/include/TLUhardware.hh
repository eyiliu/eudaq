#ifndef H_TLUHARDWARE_HH
#define H_TLUHARDWARE_HH

#include <string>
#include <iostream>
#include "i2cBus.hh"

/*
  This file contains classes for chips used on the new TLU design.
  Paolo.Baesso@bristol.ac.uk
*/
//namespace tluHw{

  class AD5665R{
  private:
    char m_i2cAddr;
    int m_DACn;
    i2cCore * m_DACcore;
    //i2cCore myi2c;
  public:
    //AD5665R::AD5665R()
    void testme();
    AD5665R();
    void SetI2CPar( i2cCore  *mycore , char addr);
    void SetIntRef(bool intRef, bool verbose);
    void SetDACValue(unsigned char channel, uint32_t value);
  };

//}
#endif
