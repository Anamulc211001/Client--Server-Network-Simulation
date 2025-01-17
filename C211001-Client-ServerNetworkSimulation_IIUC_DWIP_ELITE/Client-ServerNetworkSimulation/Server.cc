#include "IIUC_DWIP_ELITE_m.h"

using namespace omnetpp;

/**
 * The server computer; see NED file for more info
 */
class Server : public cSimpleModule
{
  private:
    cModuleType *srvProcType;
    int counter = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Server);

void Server::initialize()
{
    srvProcType = cModuleType::find("ServerProcess");
}

void Server::handleMessage(cMessage *msg)
{
    IIUC_DWIP_ELITE *pk = check_and_cast<IIUC_DWIP_ELITE *>(msg);

    if (pk->getKind() == IIUC_CONN_REQ) {
        std::string name = opp_stringf("conn-%d", ++counter);
        cModule *mod = srvProcType->createScheduleInit(name.c_str(), this);
        EV << "IIUC_CONN_REQ: Created process ID=" << mod->getId() << endl;
        sendDirect(pk, mod, "in");
    }
    else {
        int serverProcId = pk->getServerProcId();
        EV << "Redirecting msg to process ID=" << serverProcId << endl;
        cModule *mod = getSimulation()->getModule(serverProcId);
        if (!mod) {
            EV << " That process already exited, deleting msg\n";
            delete pk;
        }
        else {
            sendDirect(pk, mod, "in");
        }
    }
}
