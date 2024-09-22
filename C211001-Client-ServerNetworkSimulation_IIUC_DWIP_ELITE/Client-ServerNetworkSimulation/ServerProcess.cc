#include "IIUC_DWIP_ELITE_m.h"

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * IIUCmically launched process in the server; see NED file for more info
 */
class ServerProcess : public cSimpleModule
{
  public:
    ServerProcess() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(ServerProcess);

void ServerProcess::activity()
{
    // retrieve parameters
    cPar& processingTime = getParentModule()->par("processingTime");

    cGate *serverOutGate = getParentModule()->gate("port$o");

    int clientAddr = 0, ownAddr = 0;
    WATCH(clientAddr);
    WATCH(ownAddr);

    IIUC_DWIP_ELITE *pk;
    IIUC_DWIP_ELITEDataPacket *datapk;

    // receive the CONN_REQ we were created to handle
    EV << "Started, waiting for IIUC_CONN_REQ\n";
    pk = check_and_cast<IIUC_DWIP_ELITE *>(receive());
    clientAddr = pk->getSrcAddress();
    ownAddr = pk->getDestAddress();

    // respond to CONN_REQ by CONN_ACK
    EV << "client is addr=" << clientAddr << ", sending IIUC_CONN_ACK\n";
    pk->setName("IIUC_CONN_ACK");
    pk->setKind(IIUC_CONN_ACK);
    pk->setSrcAddress(ownAddr);
    pk->setDestAddress(clientAddr);
    pk->setServerProcId(getId());
    sendDirect(pk, serverOutGate);

    // process data packets until DISC_REQ comes
    for ( ; ; ) {
        EV << "waiting for DATA(query) (or IIUC_DISC_REQ)\n";
        pk = check_and_cast<IIUC_DWIP_ELITE *>(receive());
        int type = pk->getKind();

        if (type == IIUC_DISC_REQ)
            break;

        if (type != IIUC_DATA)
            throw cRuntimeError("protocol error!");

        datapk = (IIUC_DWIP_ELITEDataPacket *)pk;

        EV << "got DATA(query), processing...\n";
        wait((double)processingTime);

        EV << "sending DATA(result)\n";
        datapk->setName("DATA(result)");
        datapk->setKind(IIUC_DATA);
        datapk->setSrcAddress(ownAddr);
        datapk->setDestAddress(clientAddr);
        datapk->setPayload("result");
        sendDirect(datapk, serverOutGate);
    }

    // connection teardown in response to DISC_REQ
    EV << "got IIUC_DISC_REQ, sending IIUC_DISC_ACK\n";
    pk->setName("IIUC_DISC_ACK");
    pk->setKind(IIUC_DISC_ACK);
    pk->setSrcAddress(ownAddr);
    pk->setDestAddress(clientAddr);
    sendDirect(pk, serverOutGate);

    EV << "exiting\n";
    deleteModule();
}
