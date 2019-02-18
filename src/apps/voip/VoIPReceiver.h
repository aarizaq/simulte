//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_VOIPRECEIVER_H_
#define _LTE_VOIPRECEIVER_H_

#include <list>
#include <string.h>

#include <omnetpp.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet4_compat/transportlayer/contract/udp/UDPSocket.h>

#include "apps/voip/VoipPacket_m.h"

class VoIPReceiver : public inet::cSimpleModule
{
    inet::UDPSocket socket;

    ~VoIPReceiver();

    int emodel_Ie_;
    int emodel_Bpl_;
    int emodel_A_;
    double emodel_Ro_;

    typedef std::list<VoipPacket*> PacketsList;
    PacketsList mPacketsList_;
    PacketsList mPlayoutQueue_;
    unsigned int mCurrentTalkspurt_;
    unsigned int mBufferSpace_;
    inet::simtime_t mSamplingDelta_;
    inet::simtime_t mPlayoutDelay_;

    bool mInit_;

    inet::simsignal_t voIPFrameLossSignal_;
    inet::simsignal_t voIPFrameDelaySignal_;
    inet::simsignal_t voIPPlayoutDelaySignal_;
    inet::simsignal_t voIPMosSignal_;
    inet::simsignal_t voIPTaildropLossSignal_;
    inet::simsignal_t voIPPlayoutLossSignal_;
    inet::simsignal_t voIPJitterSignal_;
    inet::simsignal_t voipReceivedThroughtput_;

    virtual void finish() override;

  protected:

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessage(inet::cMessage *msg) override;
    double eModel(inet::simtime_t delay, double loss);
    void playout(bool finish);
};

#endif

