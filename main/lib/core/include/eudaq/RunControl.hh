#ifndef EUDAQ_INCLUDED_RunControl
#define EUDAQ_INCLUDED_RunControl

#include "eudaq/TransportServer.hh"
#include "eudaq/Logger.hh"
#include "eudaq/Status.hh"
#include "eudaq/Configuration.hh"
#include "eudaq/Platform.hh"
#include "eudaq/Factory.hh"

#include <string>
#include <memory>
#include <thread>
#include <mutex>

namespace eudaq {

  class RunControl;
#ifndef EUDAQ_CORE_EXPORTS
  extern template class DLLEXPORT Factory<RunControl>;
  extern template DLLEXPORT
  std::map<uint32_t, typename Factory<RunControl>::UP_BASE (*)(const std::string&)>&
  Factory<RunControl>::Instance<const std::string&>();  
#endif
  
  /** Implements the functionality of the Run Control application.
   *
   */
  class DLLEXPORT RunControl {
  public:
    explicit RunControl(const std::string &listenaddress = "");
    virtual ~RunControl(){};

    //run in user thread
    virtual void Initialise();
    virtual void Configure();
    virtual void StartRun(); 
    virtual void StopRun();
    virtual void Reset();
    virtual void RemoteStatus();
    virtual void Terminate();
    //
    
    //run in m_thd_server thread
    virtual void DoConnect(ConnectionSPC con) {}
    virtual void DoDisconnect(ConnectionSPC con) {}
    virtual void DoStatus(ConnectionSPC con,
			  std::shared_ptr<const Status> st) {}
    //
    //thread control
    void StartRunControl(); 
    void CloseRunControl();
    bool IsActiveRunControl() const {return m_thd_server.joinable();}
    virtual void Exec();
    //

    void SetRunNumber(uint32_t n){m_runnumber = n;};
    uint32_t GetRunNumber() const {return m_runnumber;};
    void ReadConfigureFile(const std::string &path);
    void ReadInitilizeFile(const std::string &path);
    std::shared_ptr<const Configuration> GetConfiguration() const {return m_conf;};
    std::shared_ptr<const Configuration> GetInitConfiguration() const {return m_conf_init;};
    
  private:
    void InitLog(std::shared_ptr<const ConnectionInfo> id);
    void SendCommand(const std::string &cmd,
		     const std::string &param = "",
                     const ConnectionSPC id = ConnectionSPC());
    void CommandHandler(TransportEvent &ev);
    void CommandThread();

  private:
    bool m_exit;
    bool m_listening;
    std::thread m_thd_server;
    std::unique_ptr<TransportServer> m_cmdserver; ///< Transport for sending commands
    std::shared_ptr<Configuration> m_conf;
    std::shared_ptr<Configuration> m_conf_init;
    std::map<std::string, std::string> m_addr_data;
    std::map<std::string, std::shared_ptr<Status>> m_status;
    std::string m_addr_log;
    std::string m_var_file;
    std::mutex m_mtx_sendcmd;
    uint32_t m_runnumber;    
  };
}

#endif // EUDAQ_INCLUDED_RunControl
