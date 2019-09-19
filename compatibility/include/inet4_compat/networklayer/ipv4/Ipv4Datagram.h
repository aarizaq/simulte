#ifndef INET4_COMPAT_IPV4DATAGRAMM_H_
#define INET4_COMPAT_IPV4DATAGRAMM_H_

#include <cassert>
#include <inet/common/packet/Packet.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>
#include <inet/transportlayer/common/L4Tools.h>
#include <inet4_compat/networklayer/contract/ipv4/Ipv4Address.h>

namespace inet {
    constexpr int IP_HEADER_BYTES = 20;
    class IPv4Datagram : public inet::Packet {
        private:
            auto getHeader() const -> decltype(peekAtFront<Ipv4Header>()) {
                auto header = this->peekAtFront<Ipv4Header>();
                return header;
            }
        public:
            explicit IPv4Datagram(const char * name = nullptr, short kind = 0) : Packet(name, kind) {};

            auto getTransportProtocol() const ->decltype(getHeader()->getProtocolId()) {
                const auto header = getHeader();
                return header->getProtocolId();
            }

            IPv4Address getSrcAddress() const {
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
                auto header = makeShared<Ipv4Header>(*getHeader());
                header->setDestAddress(addr);
                this->popAtFront<Ipv4Header>();
                this->insertAtFront(header);
            }

            void setSrcAddress(const IPv4Address& addr) {
                auto header = makeShared<Ipv4Header>(*getHeader());
                header->setSrcAddress(addr);
                this->popAtFront<Ipv4Header>();
                this->insertAtFront(header);
            }

            void setTypeOfService(short typeOfService) {
                auto header = makeShared<Ipv4Header>(*getHeader());
                header->setTypeOfService(typeOfService);
                this->popAtFront<Ipv4Header>();
                this->insertAtFront(header);
            }
    };
}
#endif
