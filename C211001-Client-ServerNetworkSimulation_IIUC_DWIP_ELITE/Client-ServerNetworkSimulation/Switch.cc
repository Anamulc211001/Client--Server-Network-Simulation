#include "IIUC_DWIP_ELITE_m.h"

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Simulates a switch between clients and server; see NED file for more info
 */
class Switch : public cSimpleModule
{
  public:
    Switch() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(Switch);

void Switch::activity()
{
    simtime_t pkDelay = 1 / (double)par("pkRate");
    int queueMaxLen = (int)par("queueMaxLen");
    cQueue queue("queue");
    for ( ; ; ) {
        // receive msg
        cMessage *msg;
        if (!queue.isEmpty())
            msg = (cMessage *)queue.pop();
        else
            msg = receive();

        // model processing delay; packets that arrive meanwhile are queued
        waitAndEnqueue(pkDelay, &queue);

        // send msg to destination
        IIUC_DWIP_ELITE *pk = check_and_cast<IIUC_DWIP_ELITE *>(msg);
        int dest = pk->getDestAddress();
        EV << "Relaying msg to addr=" << dest << "\n";
        send(msg, "port$o", dest);

        // display status: normal=queue empty, yellow=queued packets; red=queue overflow
        int qLen = queue.getLength();
        if (hasGUI())
            getDisplayString().setTagArg("i", 1, qLen == 0 ? "" : qLen < queueMaxLen ? "gold" : "red");

        // model finite queue size
        while (queue.getLength() > queueMaxLen) {
            EV << "Buffer overflow, discarding " << queue.front()->getName() << endl;
            delete queue.pop();
        }
    }
}
