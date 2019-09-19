//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "epc/gtp/GtpUserSimplified.h"
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>
#include <inet4_compat/common/cPacketToPacket.h>
#include <iostream>

Define_Module(GtpUserSimplified);

using namespace omnetpp;
using namespace inet;

void GtpUserSimplified::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    // wait until all the IP addresses are configured
    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;
    localPort_ = par("localPort");

    // get reference to the binder
    binder_ = getBinder();

    socket_.setOutputGate(gate("socketOut"));
    socket_.bind(localPort_);

    tunnelPeerPort_ = par("tunnelPeerPort");

    // get the address of the PGW
    pgwAddress_ = L3AddressResolver().resolve("pgw");

    ownerType_ = selectOwnerType(getAncestorPar("nodeType"));

    ie = detectInterface();
}

InterfaceEntry *GtpUserSimplified::detectInterface()
{
    IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    const char *interfaceName = par("ipOutInterface");
    InterfaceEntry *ie = nullptr;

    if (strlen(interfaceName) > 0) {
        ie = ift->getInterfaceByName(interfaceName);
        if (ie == nullptr)
            throw cRuntimeError("Interface \"%s\" does not exist", interfaceName);
    }

    return ie;
}

EpcNodeType GtpUserSimplified::selectOwnerType(const char * type)
{
    EV << "GtpUserSimplified::selectOwnerType - setting owner type to " << type << endl;
    if(strcmp(type,"ENODEB") == 0)
        return ENB;
    else if(strcmp(type,"PGW") == 0)
        return PGW;

    error("GtpUserSimplified::selectOwnerType - unknown owner type [%s]. Aborting...",type);
    return ENB; // should never be reached
}

void GtpUserSimplified::handleMessage(cMessage *msg)
{
    if (strcmp(msg->getArrivalGate()->getFullName(), "trafficFlowFilterGate") == 0)
    {
        EV << "GtpUserSimplified::handleMessage - message from trafficFlowFilter" << endl;
        // forward the encapsulated Ipv4 datagram
        handleFromTrafficFlowFilter(check_and_cast<Packet *>(msg));
    }
    else if(strcmp(msg->getArrivalGate()->getFullName(),"socketIn")==0)
    {
        EV << "GtpUserSimplified::handleMessage - message from udp layer" << endl;

        handleFromUdp(check_and_cast<Packet *>(msg));
    }
}

void GtpUserSimplified::handleFromTrafficFlowFilter(Packet * datagram)
{
    // extract control info from the datagram
    /* TftControlInfo * tftInfo = check_and_cast<TftControlInfo *>(datagram->removeControlInfo());
    TrafficFlowTemplateId flowId = tftInfo->getTft();
    delete (tftInfo); */

    TftControlInfo * tftInfo = datagram->removeTag<TftControlInfo>();
    TrafficFlowTemplateId flowId = tftInfo->getTft();
    delete (tftInfo);

    EV << "GtpUserSimplified::handleFromTrafficFlowFilter - Received a tftMessage with flowId[" << flowId << "]" << endl;

    // If we are on the eNB and the flowId represents the ID of this eNB, forward the packet locally
    if (flowId == 0)
    {
        // local delivery
        send(datagram,"pppGate");
    }
    else
    {
        // create a new GtpUserSimplifiedMessage
        // GtpUserMsg * gtpMsg = new GtpUserMsg();
        // gtpMsg->setName("GtpUserMessage");

        // encapsulate the datagram within the GtpUserSimplifiedMessage
        // gtpMsg->encapsulate(datagram);
        auto header = makeShared<GtpUserMsg>();
        // gtpMsg->setName("GtpUserMessage");
        header->setTeid(0);
        header->setChunkLength(B(8));
        auto gtpPacket = new Packet("GtpUserMsg");
        gtpPacket->insertAtFront(header);
        auto data = datagram->peekData();
        gtpPacket->insertAtBack(data);

        L3Address tunnelPeerAddress;
        if (flowId == -1) // send to the PGW
        {
            tunnelPeerAddress = pgwAddress_;
        }
        else
        {
            // get the symbolic IP address of the tunnel destination ID
            // then obtain the address via IPvXAddressResolver
            const char* symbolicName = binder_->getModuleNameByMacNodeId(flowId);
            tunnelPeerAddress = L3AddressResolver().resolve(symbolicName);
        }
        socket_.sendTo(gtpPacket, tunnelPeerAddress, tunnelPeerPort_);
    }
}

void GtpUserSimplified::handleFromUdp(Packet * pkt)
{
    EV << "GtpUserSimplified::handleFromUdp - Decapsulating and sending to local connection." << endl;

    // obtain the original IP datagram and send it to the local network
    // Packet * datagram = check_and_cast<Packet*>(gtpMsg->decapsulate());
    // delete(gtpMsg);
    auto originalPacket = new Packet ("OriginalDatagram");
    auto gtpUserMsg = pkt->popAtFront<GtpUserMsg>();
    originalPacket->insertAtBack(pkt->peekData());

    if (ownerType_ == PGW)
    {
        const auto& hdr = originalPacket->peekAtFront<Ipv4Header>();
        const Ipv4Address& destAddr = hdr->getDestAddress();
        MacNodeId destId = binder_->getMacNodeId(destAddr);
        if (destId != 0)
        {
             // create a new GtpUserSimplifiedMessage
             // GtpUserMsg * gtpMsg = new GtpUserMsg();
             // gtpMsg->setName("GtpUserMessage");

             // auto gtpMsg = makeShared<GtpUserMsg>();
             // gtpMsg->setName("GtpUserMessage");
             // pkt->insertAtFront(gtpMsg);

             // encapsulate the datagram within the GtpUserSimplifiedMessage
             // gtpMsg->encapsulate(datagram);
             auto header = makeShared<GtpUserMsg>();
             // gtpMsg->setName("GtpUserMessage");
             auto gtpPacket = new Packet("GtpUserMsg");
             gtpPacket->insertAtFront(header);
             auto data = originalPacket->peekData();
             gtpPacket->insertAtBack(data);

             MacNodeId destMaster = binder_->getNextHop(destId);
             const char* symbolicName = binder_->getModuleNameByMacNodeId(destMaster);
             L3Address tunnelPeerAddress = L3AddressResolver().resolve(symbolicName);
             socket_.sendTo(gtpPacket, tunnelPeerAddress, tunnelPeerPort_);
             return;
        }
    }

    // and add Interface-Request for LteNic
    if (ie != nullptr)
        originalPacket->addTagIfAbsent<InterfaceReq>()->setInterfaceId(ie->getInterfaceId());

    // send to network layer for further processing
    send(originalPacket,"pppGate");
}
