#ifndef EUDAQ_INCLUDED_LogReceiver
#define EUDAQ_INCLUDED_LogReceiver

#include "eudaq/TransportServer.hh"

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <atomic>
#include <future>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <type_traits>

namespace eudaq {
  class LogReceiver;
  using LogReceiverSP = Factory<LogReceiver>::SP_BASE;

  class DLLEXPORT LogReceiver{
  public:
    LogReceiver();
    virtual ~LogReceiver();
    virtual void OnConnect(ConnectionSPC id);
    virtual void OnDisconnect(ConnectionSPC id);
    virtual void OnReceive(ConnectionSPC id, EventSP ev);
    std::string Listen(const std::string &addr);
    void StopListen();
  private:
    void DataHandler(TransportEvent &ev);
    bool Deamon();
    bool AsyncReceiving();
    bool AsyncForwarding();
    
  private:
    std::unique_ptr<TransportServer> m_dataserver;
    std::string m_last_addr;
    bool m_is_destructing;
    bool m_is_listening;
    std::future<bool> m_fut_async_rcv;
    std::future<bool> m_fut_async_fwd;
    std::future<bool> m_fut_deamon;
    std::mutex m_mx_qu_ev;
    std::mutex m_mx_deamon;
    std::queue<std::pair<EventSP, ConnectionSPC>> m_qu_ev;
    std::condition_variable m_cv_not_empty;
  };
  //----------DOC-MARK-----END*DEC-----DOC-MARK----------
}

#endif // EUDAQ_INCLUDED_LogReceiver
