#ifndef IPV4DATAGRAMM_H_
#define IPV4DATAGRAMM_H_

#include <cassert>
#include <inet/common/packet/Packet.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>
#include <inet/transportlayer/common/L4Tools.h>
#include <inet4_compat/networklayer/contract/ipv4/IPv4Address.h>

namespace inet {
    constexpr int IP_HEADER_BYTES = 20;
    class IPv4Datagram : public Packet {
        public:
            explicit IPv4Datagram(const char * name = nullptr, short kind = 0) : Packet(name, kind) {};
            int getTransportProtocol() {
                return getHeader()->getProtocolId();
            }

            IPv4Address getSrcAddress() {
                return getHeader()->getSrcAddress();
            }

            IPv4Address getDestAddress() {
                return getHeader()->getDestAddress();
            }

            int getHeaderLength() {
                auto length = getHeader()->getHeaderLength();
                return length.get();
            }

            void setDestAddress(const IPv4Address& addr) {
                /**
                 * TODO: proper setting of IPv4 stuff. I think, the header
                 * has to be popped from the front, modified and reattached,
                 * with the changes that inet4 brought
                 */
                getHeader()->setDestAddress(addr);
            }

        private:
            const Ipv4Header* getHeader() const {
                const Ipv4Header* header =  check_and_cast<const Ipv4Header *>(getTransportProtocolHeader(this).get());
                assert(header != nullptr);
                return header;
            }

            Ipv4Header* getHeader() {
                const Ipv4Header* header =  check_and_cast<Ipv4Header *>(getTransportProtocolHeader(this).get());
                assert(header != nullptr);
                return header;
            }
    };
}
#endif
