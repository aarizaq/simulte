//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//
//

#ifndef _LTE_VODUDPCLIENT_H_
#define _LTE_VODUDPCLIENT_H_

#include <omnetpp.h>
#include <string.h>
#include <fstream>

#include <apps/vod/VoDPacket_m.h>
#include <apps/vod/VoDUDPStruct.h>
#include <inet4_compat/transportlayer/contract/udp/UDPControlInfo.h>
#include <inet4_compat/transportlayer/contract/udp/UDPSocket.h>
#include <inet/networklayer/common/L3AddressResolver.h>

class VoDUDPClient : public inet::cSimpleModule
{
    inet::UDPSocket socket;
    std::fstream outfile;
    unsigned int totalRcvdBytes_;

  public:
    inet::simsignal_t tptLayer0_;
    inet::simsignal_t tptLayer1_;
    inet::simsignal_t tptLayer2_;
    inet::simsignal_t tptLayer3_;
    inet::simsignal_t delayLayer0_;
    inet::simsignal_t delayLayer1_;
    inet::simsignal_t delayLayer2_;
    inet::simsignal_t delayLayer3_;

  protected:

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void finish() override;
    virtual void handleMessage(inet::cMessage *msg) override;
    virtual void receiveStream(VoDPacket *msg);
};

#endif

