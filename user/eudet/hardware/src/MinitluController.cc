#include "MinitluController.hh"
#include "TLUhardware.hh"
#include "i2cBus.hh"
#include <iomanip>
#include <thread>
#include <chrono>
#include <bitset>

#include "uhal/uhal.hpp"

namespace tlu {
  miniTLUController::miniTLUController(const std::string & connectionFilename, const std::string & deviceName) : m_hw(0), m_DACaddr(0), m_IDaddr(0) {

    //m_i2c = new i2cCore(connectionFilename, deviceName);
    std::cout << "Configuring from " << connectionFilename << " the device " << deviceName << std::endl;
    if(!m_hw) {
      ConnectionManager manager ( connectionFilename );
      m_hw = new uhal::HwInterface(manager.getDevice( deviceName ));
      m_i2c = new i2cCore(m_hw);
      GetFW();

    }
  }

  void miniTLUController::SetWRegister(const std::string & name, int value) {
    try {
    m_hw->getNode(name).write(static_cast< uint32_t >(value));
    m_hw->dispatch();
    } catch (...) {
      return;
    }
  }

  uint32_t miniTLUController::ReadRRegister(const std::string & name) {
    try {
    ValWord< uint32_t > test = m_hw->getNode(name).read();
    m_hw->dispatch();
    if(test.valid()) {
      return test.value();
    } else {
      std::cout << "Error reading " << name << std::endl;
      return 0;
    }
    } catch (...) {
       return 0;
    }
  }


  void miniTLUController::ReceiveEvents(){
    // std::cout<<"miniTLUController::ReceiveEvents"<<std::endl;
    uint32_t nevent = GetEventFifoFillLevel()/4;
    // std::cout<< "fifo "<<GetEventFifoFillLevel()<<std::endl;
    if (nevent*4 == 0x7D00) std::cout << "WARNING! miniTLU hardware FIFO is full" << std::endl;
    // if(0){ // no read
    if(nevent){
      ValVector< uint32_t > fifoContent = m_hw->getNode("eventBuffer.EventFifoData").readBlock(nevent*4);
      m_hw->dispatch();
      if(fifoContent.valid()) {
	std::cout<< "require events: "<<nevent<<" received events "<<fifoContent.size()/4<<std::endl;
	if(fifoContent.size()%4 !=0){
	  std::cout<<"receive error"<<std::endl;
	}
	for ( std::vector<uint32_t>::const_iterator i ( fifoContent.begin() ); i!=fifoContent.end(); i+=4 ) { //0123
	  m_data.push_back(new minitludata(*i, *(i+1), *(i+2), *(i+3)));
	  std::cout<< *(m_data.back());
	}
      }
    }
  }

  void miniTLUController::ResetEventsBuffer(){
    for(auto &&i: m_data){
      delete i;
    }
    m_data.clear();
  }

  minitludata* miniTLUController::PopFrontEvent(){
    minitludata *e = m_data.front();
    m_data.pop_front();
    return e;
  }

///////////////////////////////////

	uint32_t miniTLUController::I2C_enable(char EnclustraExpAddr)
	// This must be executed at least once after powering up the TLU or the I2C bus will not work.
	{
		std::cout << "  Enabling I2C bus" << std::endl;
		m_i2c->WriteI2CChar(EnclustraExpAddr, 0x01, 0x7F);//here
		char res= m_i2c->ReadI2CChar(EnclustraExpAddr, 0x01);//here
		std::bitset<8> resbit(res);
		if (resbit.test(7))
		{
			std::cout << "\tWarning: enabling Enclustra I2C bus might have failed. This could prevent from talking to the I2C slaves on the TLU." << int(res) << std::endl;
		}else{
			std::cout << "\tSuccess." << std::endl;
		}
	}

  uint32_t miniTLUController::GetFW(){
    uint32_t res;
    res= ReadRRegister("version");
    std::cout << "TLU FIRMWARE VERSION= " << std::hex<< res <<std::dec<< std::endl;
    return res;
  }

  uint64_t miniTLUController::getSN(){
    m_IDaddr= m_I2C_address.EEPROM;
    for(unsigned char myaddr = 0xfa; myaddr > 0x0; myaddr++) {
      char nibble = m_i2c->ReadI2CChar(m_IDaddr, myaddr);//here
      m_BoardID = ((((uint64_t)nibble)&0xff)<<((0xff-myaddr)*8))|m_BoardID;
    }
    std::cout << "  TLU Unique ID : " << std::setw(12) << std::setfill('0') << std::hex << m_BoardID << std::endl;
    return m_BoardID;
  }

  void miniTLUController::InitializeClkChip(){
    std::vector< std::vector< unsigned int> > tmpConf;
    m_zeClock.SetI2CPar(m_i2c, m_I2C_address.clockChip);
    m_zeClock.getDeviceVersion();
    m_zeClock.checkDesignID();
    std::string filename = "/users/phpgb/workspace/myFirmware/AIDA/bitFiles/TLU_CLK_Config.txt";
    tmpConf= m_zeClock.parse_clk(filename, false);
    m_zeClock.writeConfiguration(tmpConf, false);
  }

  void miniTLUController::InitializeDAC() {
    m_zeDAC1.SetI2CPar(m_i2c, m_I2C_address.DAC1);
    m_zeDAC1.SetIntRef(false, true);
    m_zeDAC2.SetI2CPar(m_i2c, m_I2C_address.DAC2);
    m_zeDAC2.SetIntRef(false, true);
    //std::cout << "  I/O expander: initialized" << std::endl;
  }

  void miniTLUController::InitializeIOexp(){
    m_IOexpander1.SetI2CPar(m_i2c, m_I2C_address.expander1);
    m_IOexpander2.SetI2CPar(m_i2c, m_I2C_address.expander2);

    //EPX1 bank 0
    m_IOexpander1.setInvertReg(0, 0x00, false); //0= normal, 1= inverted
    m_IOexpander1.setIOReg(0, 0xFF, false); // 0= output, 1= input
    m_IOexpander1.setOutputs(0, 0xFF, false); // If output, set to 1
    //EPX1 bank 1
    m_IOexpander1.setInvertReg(1, 0x00, false); // 0= normal, 1= inverted
    m_IOexpander1.setIOReg(1, 0xFF, false);// 0= output, 1= input
    m_IOexpander1.setOutputs(1, 0xFF, false); // If output, set to 1

    //EPX2 bank 0
    m_IOexpander2.setInvertReg(0, 0x00, false);// 0= normal, 1= inverted
    m_IOexpander2.setIOReg(0, 0xFF, false);// 0= output, 1= input
    m_IOexpander2.setOutputs(0, 0xFF, false);// If output, set to 1
    //EPX2 bank 1
    m_IOexpander2.setInvertReg(1, 0x00, false);// 0= normal, 1= inverted
    m_IOexpander2.setIOReg(1, 0x5F, false);// 0= output, 1= input
    m_IOexpander2.setOutputs(1, 0xFF, false);// If output, set to 1
    std::cout << "  I/O expanders: initialized" << std::endl;
  }

  void miniTLUController::InitializeI2C() {
    std::ios::fmtflags coutflags( std::cout.flags() );// Store cout flags to be able to restore them

    SetI2CClockPrescale(0x30);
    SetI2CClockPrescale(0x30);
    SetI2CControl(0x80);
    SetI2CControl(0x80);



    //char DACaddr= m_I2C_address.DAC1;
    //char IDaddr= m_I2C_address.EEPROM;

    //First we need to enable the enclustra I2C expander or we will not see any I2C slave past on the TLU
    I2C_enable(m_I2C_address.core);

    std::cout << "  Scan I2C bus:" << std::endl;
    for(int myaddr = 0; myaddr < 128; myaddr++) {
      SetI2CTX((myaddr<<1)|0x0);
      SetI2CCommand(0x90); // 10010000
	    while(I2CCommandIsDone()) { // xxxxxx1x   TODO:: isDone or notDone
        std::this_thread::sleep_for(std::chrono::seconds(1));
	    }
	    bool isConnected = (((GetI2CStatus()>>7)&0x1) == 0);  // 0xxxxxxx connected
      if (isConnected){
        if (myaddr== m_I2C_address.core){
          //std::cout << "\tFOUND I2C slave CORE" << std::endl;
        }
        else if (myaddr== m_I2C_address.clockChip){
          std::cout << "\tFOUND I2C slave CLOCK" << std::endl;
        }
        else if (myaddr== m_I2C_address.DAC1){
          std::cout << "\tFOUND I2C slave DAC1" << std::endl;
        }
        else if (myaddr== m_I2C_address.DAC2){
          std::cout << "\tFOUND I2C slave DAC2" << std::endl;
        }
        else if (myaddr==m_I2C_address.EEPROM){
          m_IDaddr= myaddr;
          std::cout << "\tFOUND I2C slave EEPROM" << std::endl;
        }
        else if (myaddr==m_I2C_address.expander1){
          std::cout << "\tFOUND I2C slave EXPANDER1" << std::endl;
        }
        else if (myaddr==m_I2C_address.expander2){
          std::cout << "\tFOUND I2C slave EXPANDER2" << std::endl;
        }
        else{
          std::cout << "\tI2C slave at address 0x" << std::hex << myaddr << " replied but is not on TLU address list. A mistery!" << std::endl;
        }
      }
      SetI2CTX(0x0);
  		SetI2CCommand(0x50); // 01010000
  		while(I2CCommandIsDone()) {
  			std::this_thread::sleep_for(std::chrono::seconds(1));
  		}
    }

    if(m_IDaddr){
      getSN();
    }
    std::cout.flags( coutflags ); // Restore cout flags
  }

/*
  void miniTLUController::WriteI2CChar(char deviceAddr, char memAddr, char value) {
	SetI2CTX((deviceAddr<<1)|0x0);
    SetI2CCommand(0x90);
    while(I2CCommandIsDone()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SetI2CTX(memAddr);
    SetI2CCommand(0x10);
    while(I2CCommandIsDone()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SetI2CTX(value);
    SetI2CCommand(0x50);
    while(I2CCommandIsDone()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
*/
/*
  void miniTLUController::WriteI2CCharArray(char deviceAddr, char memAddr, unsigned char *values, unsigned int len) {
    unsigned int i;

    SetI2CTX((deviceAddr<<1)|0x0);
    SetI2CCommand(0x90);
    while(I2CCommandIsDone()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SetI2CTX(memAddr);
    SetI2CCommand(0x10);
    while(I2CCommandIsDone()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    for(i = 0; i < len-1; ++i) {
		SetI2CTX(values[i]);
		SetI2CCommand(0x10);
		while(I2CCommandIsDone()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
    }
    SetI2CTX(values[len-1]);
    SetI2CCommand(0x50);
    while(I2CCommandIsDone()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
*/
/*
  char miniTLUController::ReadI2CChar(char deviceAddr, char memAddr) {
    SetI2CTX((deviceAddr<<1)|0x0);
    SetI2CCommand(0x90);
    while(I2CCommandIsDone()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SetI2CTX(memAddr);
    SetI2CCommand(0x10);
    while(I2CCommandIsDone()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SetI2CTX((deviceAddr<<1)|0x1);
    SetI2CCommand(0x90);
    while(I2CCommandIsDone()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    SetI2CCommand(0x28);
    while(I2CCommandIsDone()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return GetI2CRX();
  }
*/
/*
  void miniTLUController::SetDACValue(unsigned char channel, uint32_t value) {
    unsigned char chrsToSend[2];

    std::cout << "Setting DAC channel " << (unsigned int)channel << " = " << value << std::endl;

    chrsToSend[0] = 0x0;
    chrsToSend[1] = 0x0;
    //chrsToSend[1] = 0x1;
    m_i2c->WriteI2CCharArray(m_DACaddr, 0x38, chrsToSend, 2);//here

    // set the value
    chrsToSend[1] = value&0xff;
    chrsToSend[0] = (value>>8)&0xff;
    m_i2c->WriteI2CCharArray(m_DACaddr, 0x18+(channel&0x7), chrsToSend, 2);//here
  }
*/
  void miniTLUController::SetThresholdValue(unsigned char channel, float thresholdVoltage ) {
    //Channel can either be [0, 5] or 7 (all channels).
    int nChannels= 6; //We should read this from conf file, ideally.
    bool intRef= false; //We should read this from conf file, ideally.
    float vref;
    if (intRef){
      vref = 2.500; // Internal reference
    }
    else{
      vref = 1.300; // Reference voltage is 1.3V on newer TLU
    }
    if ( std::abs(thresholdVoltage) > vref ){
      thresholdVoltage= vref*thresholdVoltage/std::abs(thresholdVoltage);
      std::cout<<"\tWarning: Threshold voltage is outside range [-1.3 , +1.3] V. Coerced to "<< thresholdVoltage << " V"<<std::endl;
    }

    float vdac = ( thresholdVoltage + vref ) / 2;
    float dacCode =  0xFFFF * vdac / vref;

    if( (channel != 7) && ((channel < 0) || (channel > (nChannels-1)))  ){
      std::cout<<"\tError: DAC illegal DAC channel. Use 7 for all channels or 0 <= channel <= "<< nChannels-1 << std::endl;
      return;
    }

    if (channel==7){
      m_zeDAC1.SetDACValue(channel , int(dacCode) );
      m_zeDAC2.SetDACValue(channel , int(dacCode) );
      std::cout << "  Setting threshold for all channels to " << thresholdVoltage << " Volts" << std::endl;
      return;
    }
    if (channel <4){
      m_zeDAC2.SetDACValue(channel , int(dacCode) );
      std::cout << "  Setting threshold for channel " << (unsigned int)channel << " to " << thresholdVoltage << " Volts" << std::endl;
    }
    else{
      m_zeDAC1.SetDACValue(channel-4 , int(dacCode) );
      std::cout << "  Setting threshold for channel " << (unsigned int)channel << " to " << thresholdVoltage << " Volts" << std::endl;
    }

  }

  void miniTLUController::DumpEventsBuffer() {
    std::cout<<"miniTLUController::DumpEvents......"<<std::endl;
    for(auto&& i: m_data){
      std::cout<<i<<std::endl;
    }
    std::cout<<"miniTLUController::DumpEvents end"<<std::endl;
  }


  void miniTLUController::SetUhalLogLevel(uchar_t l){
    switch(l){
    case 0:
      uhal::disableLogging();
      break;
    case 1:
      uhal::setLogLevelTo(uhal::Fatal());
      break;
    case 2:
      uhal::setLogLevelTo(uhal::Error());
      break;
    case 3:
      uhal::setLogLevelTo(uhal::Warning());
      break;
    case 4:
      uhal::setLogLevelTo(uhal::Notice());
      break;
    case 5:
      uhal::setLogLevelTo(uhal::Info());
      break;
    case 6:
      uhal::setLogLevelTo(uhal::Debug());
      break;
    default:
      uhal::setLogLevelTo(uhal::Debug());
    }
  }



  std::ostream &operator<<(std::ostream &s, minitludata &d) {
    s << "eventnumber: " << d.eventnumber << " type: " << int(d.eventtype) <<" timestamp: 0x" <<std::hex<< d.timestamp <<std::dec<<std::endl
      <<" input0: " << int(d.input0) << " input1: " << int(d.input1) << " input2: " << int(d.input2) << " input3: " << int(d.input3) <<std::endl
      <<" sc0: " << int(d.sc0) << " sc1: "  << int(d.sc1) << " sc2: "  << int(d.sc2) << " sc3: " << int(d.sc3) <<std::endl;
    return s;
  }


}
