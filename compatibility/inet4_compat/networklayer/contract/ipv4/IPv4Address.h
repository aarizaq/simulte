#ifndef IPV4ADDRESS_H
#define IPV4ADDRESS_H

#include <inet/networklayer/contract/ipv4/Ipv4Address.h>

namespace inet {
    class IPv4Address : public Ipv4Address {
        public:
            explicit IPv4Address(const char* address) : Ipv4Address(address) {};
            explicit IPv4Address(uint32 address) : Ipv4Address(address) {};
    };
}
#endif
