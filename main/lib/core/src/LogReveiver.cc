/*

#include "eudaq/LogCollector.hh"
#include "eudaq/LogMessage.hh"
#include "eudaq/Logger.hh"
#include "eudaq/TransportServer.hh"
#include "eudaq/BufferSerializer.hh"
#include <iostream>
#include <ostream>
#include <sstream>

namespace eudaq {

  template class DLLEXPORT Factory<LogCollector>;
  template DLLEXPORT
  std::map<uint32_t, typename Factory<LogCollector>::UP_BASE (*)
	   (const std::string&, const std::string&)>&
  Factory<LogCollector>::Instance<const std::string&, const std::string&>();
  
  LogCollector::LogCollector(const std::string &name, const std::string &runcontrol)
    : CommandReceiver("LogCollector", name, runcontrol), m_exit(false){
  }
  
  LogCollector::~LogCollector(){
    CloseLogCollector();
  }

  void LogCollector::OnInitialise(){
    auto conf = GetConfiguration();
    try{
      DoInitialise();
      CommandReceiver::OnInitialise();
    }catch (const Exception &e) {
      std::string msg = "Error when init by " + conf->Name() + ": " + e.what();
      EUDAQ_ERROR(msg);
      SetStatus(Status::STATE_ERROR, msg);
    }
  }

  
  void LogCollector::OnTerminate(){
    CloseLogCollector();
    DoTerminate();
    CommandReceiver::OnTerminate();
    std::exit(0);
  }
  
  void LogCollector::TransportHandler(TransportEvent &ev) {
    auto con = ev.id;
    switch (ev.etype) {
    case (TransportEvent::CONNECT):
      m_logserver->SendPacket("OK EUDAQ LOG LogCollector", *con, true);
      break;
    case (TransportEvent::DISCONNECT):
      con->SetState(0);
      DoDisconnect(con);
      break;
    case (TransportEvent::RECEIVE):
      if (con->GetState() == 0) { // waiting for identification
        do {
          size_t i0 = 0, i1 = ev.packet.find(' ');
          if (i1 == std::string::npos)
            break;
          std::string part(ev.packet, i0, i1);
          if (part != "OK")
            break;
          i0 = i1 + 1;
          i1 = ev.packet.find(' ', i0);
          if (i1 == std::string::npos)
            break;
          part = std::string(ev.packet, i0, i1 - i0);
          if (part != "EUDAQ")
            break;
          i0 = i1 + 1;
          i1 = ev.packet.find(' ', i0);
          if (i1 == std::string::npos)
            break;
          part = std::string(ev.packet, i0, i1 - i0);
          if (part != "LOG")
            break;
          i0 = i1 + 1;
          i1 = ev.packet.find(' ', i0);
          part = std::string(ev.packet, i0, i1 - i0);
          con->SetType(part);
          i0 = i1 + 1;
          i1 = ev.packet.find(' ', i0);
          part = std::string(ev.packet, i0, i1 - i0);
          con->SetName(part);
        } while (false);
        m_logserver->SendPacket("OK", *con, true);
        con->SetState(1); // successfully identified
	std::unique_lock<std::mutex> lk(m_mx_qu_ev);
	m_qu_ev.push(std::make_pair<EventSP, ConnectionSPC>(nullptr, con));
	lk.unlock();
	m_cv_not_empty.notify_all();
      } else {
        BufferSerializer ser(ev.packet.begin(), ev.packet.end());
        std::string src = con->GetType();
        if (con->GetName() != "")
          src += "." + con->GetName();
	
	std::ostringstream buf;
	LogMessage logmesg(ser);
	logmesg.SetSender(src);
	logmesg.Write(buf);
	// m_file.write(buf.str().c_str(), buf.str().length());
	// m_file.flush();
	DoReceive(logmesg);
      }
      break;
    default:
      std::cout << "Unknown:    " << *con << std::endl;
    }
  }

  bool LogReceiver::AsyncForwarding(){
    while(m_is_listening){
      std::unique_lock<std::mutex> lk(m_mx_qu_ev);
      if(m_qu_ev.empty()){
	while(m_cv_not_empty.wait_for(lk, std::chrono::seconds(1))
	      ==std::cv_status::timeout){
	  if(!m_is_listening){
	    return 0;
	  }
	}
      }
      auto ev = m_qu_ev.front().first;
      auto con = m_qu_ev.front().second;
      m_qu_ev.pop();
      lk.unlock();
      if(ev){
	OnReceive(con, ev);
      }
      else{
	if(con->GetState())
	  OnConnect(con);
	else{
	  OnDisconnect(con);
	}
      }
    }
    return 0;
  }
  
  bool LogReceiver::AsyncReceiving(){
    while (m_is_listening){
      m_trans_server->Process(100000);
    }
    return 0;
  }

  std::string LogCollector::Listen(){
    std::string this_addr = m_last_addr;
    if(!addr.empty()){
      this_addr = addr;
    }
    if(m_trans_server){
      EUDAQ_THROW("LogReceiver:: Last server did not closed sucessfully");
    }

    auto trans_server = TransportServer::CreateServer(this_addr);
    trans_server->SetCallback(TransportCallback(this, &LogReceiver::TransportHandler));
    
    m_last_addr = logserver->ConnectionString();
    m_trans_server.reset(trans_server);
    m_is_listening = true;
    m_fut_async_rcv = std::async(std::launch::async, &DataReceiver::AsyncReceiving, this); 
    m_fut_async_fwd = std::async(std::launch::async, &DataReceiver::AsyncForwarding, this);
    return m_last_addr;
  }

  void LogReceiver::StopListen(){
    m_is_listening = false;
    auto tp_stop = std::chrono::steady_clock::now();    
    while( m_fut_async_rcv.valid() || m_fut_async_fwd.valid()){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if((std::chrono::steady_clock::now()-tp_stop) > std::chrono::seconds(10)){
	EUDAQ_THROW("LogReceiver:: Unable to stop the data receving/forwarding threads");
      }
    }
  }

}
*/
