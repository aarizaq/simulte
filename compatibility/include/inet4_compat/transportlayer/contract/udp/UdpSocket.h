#ifndef INET_COMPAT_UDPSOCKET_H_
#define INET_COMPAT_UDPSOCKET_H_

#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include <inet/common/packet/Packet.h>
#include <inet/common/packet/chunk/cPacketChunk.h>
#include <inet/transportlayer/udp/UdpHeader_m.h>

namespace inet {
    class INET_API UDPSocket : public UdpSocket {
        public: 
            void sendTo( omnetpp::cPacket * packet, 
                    const inet::L3Address& destinationAddress, 
                    int  destinationPort) {

                auto data = makeShared<cPacketChunk>(packet);
                auto header = makeShared<UdpHeader>();
                auto udpPacket = new inet::Packet(packet->getName(), data);
                udpPacket->insertAtFront(header);

                UdpSocket::sendTo(udpPacket, destinationAddress, destinationPort);
            }
    };
}



#endif
