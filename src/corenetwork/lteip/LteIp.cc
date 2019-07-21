//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include <inet4_compat/networklayer/contract/ipv4/IPv4ControlInfo.h>
#include <inet/common/ModuleAccess.h>
#include <inet/networklayer/ipv4/Ipv4InterfaceData.h>

#include "corenetwork/lteip/LteIp.h"
#include "corenetwork/binder/LteBinder.h"
#include "corenetwork/deployer/LteDeployer.h"

Define_Module(LteIp);
using namespace std;
using namespace omnetpp;
using namespace inet;

/**
 * @author: wolfgang kallies
 * TODO:this entire class has to be rewritten, since the underlying
 * mechanics in inet changed. mapping_ attribute was removed: the corresponding
 * class does not exist anymore in inet4 and the underlying mechanism of passing
 * packets has changed. 
 * (I am still struggling understanding, how, but since this thing does not 
 * compile, testing is somewhat tricky). The point being:
 * DO NOT EXPECT THIS TO WORK. And it will all end in tears (hopefully not mine).
 */
void LteIp::initialize(int stage)
{
    OperationalBase::initialize(stage);

    stackGateOut_ = gate("stackLte$o");
    peerGateOut_ = gate("ifOut");

    defaultTimeToLive_ = par("timeToLive");
    //mapping_.parseProtocolMapping(par("protocolMapping"));
    setNodeType(par("nodeType").stdstringValue());

    numDropped_ = numForwarded_ = seqNum_ = 0;
    if (nodeType_ == UE)
    {
        cModule *ue = getParentModule();
        getBinder()->registerNode(ue, nodeType_, ue->par("masterId"));
    }
    else if (nodeType_ == ENODEB)
    {
        cModule *enodeb = getParentModule();
        MacNodeId cellId = getBinder()->registerNode(enodeb, nodeType_);
        LteDeployer * deployer = check_and_cast<LteDeployer*>(enodeb->getSubmodule("deployer"));
        getBinder()->registerDeployer(deployer, cellId);
    }

    WATCH(numForwarded_);
    WATCH(numDropped_);
    WATCH(seqNum_);
}

void LteIp::updateDisplayString()
{
    char buf[80] = "";
    if (numForwarded_ > 0)
        sprintf(buf + strlen(buf), "fwd:%d ", numForwarded_);
    if (numDropped_ > 0)
        sprintf(buf + strlen(buf), "drop:%d ", numDropped_);
    getDisplayString().setTagArg("t", 0, buf);
}

void LteIp::endService(cPacket *msg)
{
    if (nodeType_ == INTERNET)
    {
        if (msg->getArrivalGate()->isName("transportIn"))
        {
            // message from transport: send to peer
            numForwarded_++;
            EV << "LteIp: message from transport: send to peer" << endl;
            fromTransport(msg, peerGateOut_);
        }
        else if (msg->getArrivalGate()->isName("ifIn"))
        {
            // message from peer: send to transport
            numForwarded_++;
            EV << "LteIp: message from peer: send to transport" << endl;
            toTransport(msg);
        }
        else
        {
            // error: drop message
            numDropped_++;
            delete msg;
            EV << "LteIp (INTERNET): Wrong gate " << msg->getArrivalGate()->getName() << endl;
        }
    }
    else if( nodeType_ == ENODEB )
    {
        if (msg->getArrivalGate()->isName("ifIn"))
        {
            // message from peer: send to stack
            numForwarded_++;
            EV << "LteIp: message from peer: send to stack" << endl;

            IPv4Datagram *ipDatagram = check_and_cast<IPv4Datagram *>(msg);
            cPacket *transportPacket = ipDatagram->getEncapsulatedPacket();

            // TODO: KLUDGE: copied over from function fromTransport
            unsigned short srcPort = 0;
            unsigned short dstPort = 0;
            int headerSize = 0;//IP_HEADER_BYTES;

            switch(ipDatagram->getTransportProtocol())
            {
                case IP_PROT_TCP:
                    inet::tcp::TCPSegment* tcpseg;
                    tcpseg = check_and_cast<inet::tcp::TCPSegment*>(transportPacket);
                    srcPort = tcpseg->getSrcPort();
                    dstPort = tcpseg->getDestPort();
                    headerSize += tcpseg->getHeaderLength();
                    break;
                case IP_PROT_UDP:
                    inet::UDPPacket* udppacket;
                    udppacket = check_and_cast<UDPPacket*>(transportPacket);
                    srcPort = (unsigned short)udppacket->getSourcePort();
                    dstPort = (unsigned short)udppacket->getDestinationPort();
                    headerSize += UDP_HEADER_BYTES;
                    break;
                default :
                    throw std::runtime_error("LteIP::endService() case not implemented");
            }

            FlowControlInfo *controlInfo = new FlowControlInfo();
            controlInfo->setSrcAddr(ipDatagram->getSrcAddress().getInt());
            controlInfo->setDstAddr(ipDatagram->getDestAddress().getInt());
            controlInfo->setSrcPort(srcPort);
            controlInfo->setDstPort(dstPort);
            controlInfo->setSequenceNumber(seqNum_++);
            controlInfo->setHeaderSize(headerSize);
            MacNodeId destId = getBinder()->getMacNodeId(IPv4Address(controlInfo->getDstAddr()));
            // master of this ue (myself or a relay)
            // TODO: KLUDGE:
            MacNodeId master = getBinder()->getNextHop(destId);
//            if (master != nodeId_) { // ue is relayed, dest must be the relay
            destId = master;
//            } // else ue is directly attached
            controlInfo->setDestId(destId);
            printControlInfo(controlInfo);
            msg->setControlInfo(controlInfo);

            send(msg,stackGateOut_);
        }
        else if(msg->getArrivalGate()->isName("stackLte$i"))
        {
            // message from stack: send to peer
            numForwarded_++;
            EV << "LteIp: message from stack: send to peer" << endl;
            send(msg,peerGateOut_);
        }
        else
        {
            // error: drop message
            numDropped_++;
            delete msg;
            EV << "LteIp (ENODEB): Wrong gate " << msg->getArrivalGate()->getName() << endl;
        }
    }
    else if( nodeType_ == UE )
    {
        if (msg->getArrivalGate()->isName("transportIn"))
        {
            // message from transport: send to stack
            numForwarded_++;
            std::cout << "LteIp: message from transport: send to stack" << endl;
            fromTransport(msg,stackGateOut_);
        }
        else if(msg->getArrivalGate()->isName("stackLte$i"))
        {
            // message from stack: send to transport
            numForwarded_++;
            std::cout << "LteIp: message from stack: send to transport" << endl;
            toTransport(msg);
        }
        else
        {
            // error: drop message
            numDropped_++;
            delete msg;
            EV << "LteIp (UE): Wrong gate " << msg->getArrivalGate()->getName() << endl;
        }
    }

    if (getEnvir()->isGUI())
    updateDisplayString();
}

void LteIp::fromTransport(cPacket * transportPacket, cGate *outputgate)
{
    /**
     * @author: wolfgang kallies
     * TODO: this needs to be largely rewritten. IPv4Controlinfo is no longer
     * used and available. Might not work after 'fixing'
     */
    // Remove control info from transport packet
    //IPv4ControlInfo *ipControlInfo = check_and_cast<IPv4ControlInfo*>(transportPacket->removeControlInfo());
    auto packetTmp = check_and_cast<inet::Packet*>(transportPacket);
    auto ipControlInfo = make_shared<Ipv4Header>(*(packetTmp->popAtFront<Ipv4Header>()));

    //** Create IP datagram and fill its fields **

    /*
     * @author: wolfgang kallies
     * I am using the default ctor and hope this works.
     */
    IPv4Datagram *datagram = new IPv4Datagram(transportPacket->getName());
    /*
     * since the interface changed in inet4, this might screw up
     * TODO: write some tests. But it has to compile first!
     */
    datagram->setByteLength(IP_HEADER_BYTES);
    datagram->encapsulate(transportPacket);

    // set destination address
    IPv4Address dest = ipControlInfo->getDestAddress();
    datagram->setDestAddress(dest);

    // set source address
    IPv4Address src = ipControlInfo->getSrcAddress();

    // when source address was given, use it; otherwise use local Addr
    if (src.isUnspecified())
    {
        // find interface entry and use its address
        IInterfaceTable *interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        // TODO: how do we find the LTE interface?
        src = interfaceTable->getInterfaceByName("wlan")->getIpv4Address();
        EV << "Local address used: " << src << endl;
    }
    else
    {
        EV << "Source address in control info from transport layer " << src << endl;
    }
    datagram->setSrcAddress(src);

    // set other fields
    datagram->setTypeOfService(ipControlInfo->getTypeOfService());
    //ipControlInfo->setTransportProtocol(ipControlInfo->getProtocol());

    // fragmentation fields are correctly filled, but not used
    ipControlInfo->setIdentification(curFragmentId_++);
    ipControlInfo->setMoreFragments(false);
    ipControlInfo->setDontFragment(ipControlInfo->getDontFragment());
    ipControlInfo->setFragmentOffset(0);

    // ttl
    short ci_ttl = ipControlInfo->getTimeToLive();
    ipControlInfo->setTimeToLive((ci_ttl > 0) ? ci_ttl : defaultTimeToLive_);

    //** Add control info for stack **
    unsigned short srcPort = 0;
    unsigned short dstPort = 0;
    int headerSize = IP_HEADER_BYTES;

    switch (ipControlInfo->getProtocolId())
    {
        case IP_PROT_TCP:
            inet::tcp::TCPSegment* tcpseg;
            tcpseg = check_and_cast<inet::tcp::TCPSegment*>(transportPacket);
            srcPort = tcpseg->getSrcPort();
            dstPort = tcpseg->getDestPort();
            headerSize += tcpseg->getHeaderLength();
            break;
        case IP_PROT_UDP:
            inet::UDPPacket* udppacket;
            udppacket = check_and_cast<inet::UDPPacket*>(transportPacket);
            srcPort = (unsigned short) udppacket->getSourcePort();
            dstPort = (unsigned short) udppacket->getDestinationPort();
            headerSize += UDP_HEADER_BYTES;
            break;
        default:
            std::string errormessage = "";
            errormessage += "LteIp::fromTransport() error: ";
            errormessage += "got unhandled Protocol\n";
            errormessage += "protocol id: " + std::to_string(ipControlInfo->getProtocolId()) + "\n";
            throw std::runtime_error(errormessage);
    }

    datagram->insertAtFront(Ptr<Ipv4Header>(ipControlInfo.get()));

    FlowControlInfo *controlInfo = new FlowControlInfo();
    controlInfo->setSrcAddr(src.getInt());
    controlInfo->setDstAddr(dest.getInt());
    controlInfo->setSrcPort(srcPort);
    controlInfo->setDstPort(dstPort);
    controlInfo->setSequenceNumber(seqNum_++);
    controlInfo->setHeaderSize(headerSize);
    printControlInfo(controlInfo);

    datagram->setControlInfo(controlInfo);
   

    //** Send datagram to lte stack or LteIp peer **
    send(datagram, outputgate);
}

void LteIp::toTransport(cPacket * msg)
{
    // msg is an IP Datagram (from Lte stack or IP peer)
    IPv4Datagram *datagram = check_and_cast<IPv4Datagram *>(msg);
    // int protocol = datagram->getTransportProtocol();

    /**
     * @author: wolfgang kallies
     * dropped, I hope this is redundant.
     */
    //int gateindex = 0;
    //try
    //{
    //    gateindex = mapping_.getOutputGateForProtocol(protocol);
    //}
    //catch (cRuntimeError)
    //{
    //    EV << "Protocol mapping failed with protocol number : " << protocol << endl;
    //    EV << "Packet dropped" << endl;
    //    delete msg;
    //    numForwarded_--;
    //    numDropped_++;
    //    return;
    //}

        // transport packet
    //cPacket *transportPacket = datagram->decapsulate();
    auto header = datagram->peekAtFront<Ipv4Header>();
    
    

    // create and fill in control info
    auto controlInfo = make_shared<Ipv4Header>();
    controlInfo->setProtocol(header->getProtocol());
    controlInfo->setSrcAddress(datagram->getSrcAddress());
    controlInfo->setDestAddress(datagram->getDestAddress());
    controlInfo->setTypeOfService(header->getTypeOfService());
    auto packet = make_shared<IPv4Datagram>(*datagram);

    // XXX these two fields (interfaceId and datagram) are not actually of interest for us..

    // interfaceId should be the interface on which the datagram arrived,
    // or -1 if it was created locally
    //controlInfo->setInterfaceId(-1);
    // original IP datagram might be needed in upper layers
    //controlInfo->setOrigDatagram(datagram);

    // attach control info
    //transportPacket->setControlInfo(controlInfo);
    packet->popAtFront<Ipv4Header>();
    packet->insertAtFront(Ptr<Ipv4Header>(controlInfo.get()));

    //send(transportPacket, "transportOut", gateindex);
    send(packet.get(), "transportOut");
}

void LteIp::setNodeType(std::string s)
{
    nodeType_ = aToNodeType(s);
    EV << "Node type: " << s << " -> " << nodeType_ << endl;
}

void LteIp::printControlInfo(FlowControlInfo* ci)
{
    EV << "Src IP : " << IPv4Address(ci->getSrcAddr()) << endl;
    EV << "Dst IP : " << IPv4Address(ci->getDstAddr()) << endl;
    EV << "Src Port : " << ci->getSrcPort() << endl;
    EV << "Dst Port : " << ci->getDstPort() << endl;
    EV << "Seq Num  : " << ci->getSequenceNumber() << endl;
    EV << "Header Size : " << ci->getHeaderSize() << endl;
}

void LteIp::handleMessageWhenUp(cMessage *msg)
{
    throw cRuntimeError(" handleMessageWhenUp method not implemented");

    /*  note: this is the implementation in the IPv4-Layer
     * TODO: check if it can be adapted/applied hiere
     *
    if (msg->arrivedOn("transportIn")) {    //TODO packet->getArrivalGate()->getBaseId() == transportInGateBaseId
        if (auto request = dynamic_cast<Request *>(msg))
            handleRequest(request);
        else
            handlePacketFromHL(check_and_cast<Packet*>(msg));
    }
    else if (msg->arrivedOn("queueIn")) {    // from network
        EV_INFO << "Received " << msg << " from network.\n";
        handleIncomingDatagram(check_and_cast<Packet*>(msg));
    }
    else
        throw cRuntimeError("message arrived on unknown gate '%s'", msg->getArrivalGate()->getName());
    */
}


/**
 * TODO: implement ILifeCycleMethods: stop should flush all pending packets (see Ipv4(?))
 */

void LteIp::handleStartOperation(LifecycleOperation *operation)
{
    throw cRuntimeError(" handleStartOperation not implemented");
}

void LteIp::handleStopOperation(LifecycleOperation *operation)
{
    throw cRuntimeError(" handleStopOperation not implemented");
}

void LteIp::handleCrashOperation(LifecycleOperation *operation)
{
    throw cRuntimeError(" handleCrashOperation not implemented");
}



