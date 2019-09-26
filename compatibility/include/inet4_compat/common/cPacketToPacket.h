/* ugly hack: conversion of omnet::cPacket <-> inet::Packet   -   should be removed as soon as simulte has been ported to fully support inet::Packet */

#ifndef INET_COMPAT_CPACKET_TO_PACKET_H_
#define INET_COMPAT_CPACKET_TO_PACKET_H_

#include <omnetpp.h>
#include <inet/common/packet/Packet.h>
#include <inet/common/packet/chunk/ByteCountChunk.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>

#include "common/LteControlInfo.h"

using namespace inet;
using namespace omnetpp;

namespace inet {

    /**
     * Convert a cPacket to an inet::Packet
     *
     * Assumption: input packet encapsulates an inet::Packet
     */
    inline inet::Packet* convertToInetPacket(omnetpp::cPacket* &in){
        // std::cout << "convertToInetPacket converting packet: " << in->getName() << endl;

        Packet* outPacket = check_and_cast<Packet*>(in->decapsulate());
        delete in;
        in = nullptr;

        // std::cout << "convertToInetPacket returning: " << outPacket->str() << endl;

        return outPacket;
    }

    /**
     * Convert a  inet::Packet to an omnetpp::cPacket
     *
     * In order to be able to convert the cPacket back to Packet lateron,
     * we encapsulate the original Packet within the cPacket
     */
    inline omnetpp::cPacket* convertTocPacket(inet::Packet* &in, omnetpp::cObject * controlInfo){
        // std::cout << "convertTocPacket converting packet: " << in->getName() << endl;
        // std::cout << "convertTocPacket      input packet: " << in->str() << endl;
        // TODO: get rid of control info and use inet::Packet instead
        cPacket* pktToLte = new cPacket(*in);
        pktToLte->encapsulate(in);
        in = nullptr;
        pktToLte->setControlInfo(controlInfo);
        return pktToLte;
    }
}

#endif
