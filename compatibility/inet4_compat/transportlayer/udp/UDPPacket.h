#ifndef INET_COMPAT_UDPPACKET_H_
#define INET_COMPAT_UDPPACKET_H_

#include <inet/common/packet/Packet.h>
#include <inet/transportlayer/udp/UdpHeader_m.h>

namespace inet {
    static constexpr int UDP_HEADER_BYTES = 8;
    class UDPPacket : public inet::Packet {
        public:
            int getSourcePort() {
                return getHeader()->getSourcePort();
            }

            int getDestinationPort() {
                return getHeader()->getDestinationPort();
            }
        private:
            const inet::UdpHeader* getHeader() {
                const auto ptr = check_and_cast<const inet::UdpHeader*>(this);
                assert(ptr != nullptr);
                return ptr;
            }
    };
};
#endif
