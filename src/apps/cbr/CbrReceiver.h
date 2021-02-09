//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _CBRRECEIVER_H_
#define _CBRRECEIVER_H_

#include <string.h>
#include <omnetpp.h>

#include <inet/common/INETDefs.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include <inet/networklayer/common/L3AddressResolver.h>

#include "common/LteCommon.h"
#include "apps/cbr/CbrPacket_m.h"

class SIMULTE_API CbrReceiver : public omnetpp::cSimpleModule
{
    inet::UdpSocket socket;

    int numReceived_;
    int totFrames_;
    int recvBytes_;

    bool mInit_;

    static omnetpp::simsignal_t cbrFrameLossSignal_;
    static omnetpp::simsignal_t cbrFrameDelaySignal_;
    static omnetpp::simsignal_t cbrJitterSignal_;
    static omnetpp::simsignal_t cbrReceivedThroughtput_;
    static omnetpp::simsignal_t cbrReceivedBytesSignal_;

    omnetpp::simsignal_t cbrRcvdPkt_;

  protected:

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
};

#endif

