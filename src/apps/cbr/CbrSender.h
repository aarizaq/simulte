//
//

#ifndef _CBRSENDER_H_
#define _CBRSENDER_H_

#include <string.h>
#include <omnetpp.h>

#include <inet/common/INETDefs.h>
//#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <inet4_compat/transportlayer/contract/udp/UDPSocket.h>
#include <inet/networklayer/common/L3AddressResolver.h>

#include "CbrPacket_m.h"

class CbrSender : public omnetpp::cSimpleModule
{
    inet::UDPSocket socket;
    //has the sender been initialized?
    bool initialized_;

    omnetpp::cMessage* selfSource_;
    //sender
    int iDtalk_;
    int nframes_;
    int iDframe_;
    int nframesTmp_;
    int size_;
    omnetpp::simtime_t sampling_time;
    omnetpp::simtime_t startTime_;
    omnetpp::simtime_t finishTime_;

    bool silences_;

    omnetpp::simsignal_t cbrGeneratedThroughtput_;
    // ----------------------------

    omnetpp::cMessage *selfSender_;
    omnetpp::cMessage *initTraffic_;

    omnetpp::simtime_t timestamp_;
    int localPort_;
    int destPort_;
    inet::L3Address destAddress_;

    void initTraffic();
    void sendCbrPacket();

  public:
    ~CbrSender();
    CbrSender();

  protected:

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
};

#endif

