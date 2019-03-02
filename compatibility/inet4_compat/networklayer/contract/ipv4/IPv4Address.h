#ifndef INET_COMPAT_IPV4ADDRESS_H_
#define INET_COMPAT_IPV4ADDRESS_H_

#include <inet/networklayer/contract/ipv4/Ipv4Address.h>

namespace inet {
    class IPv4Address : public Ipv4Address {
        public:
            IPv4Address(const Ipv4Address& add) : Ipv4Address(add) {};
            explicit IPv4Address(const char* address) : Ipv4Address(address) {};
            explicit IPv4Address(uint32 address) : Ipv4Address(address) {};
         //   enum AddressCategory {
         //       UNSPECIFIED = Ipv4Address::UNSPECIFIED,    // 0.0.0.0
         //       THIS_NETWORK = Ipv4Address::THIS_NETWORK,    // 0.0.0.0/8
         //       LOOPBACK = Ipv4Address::LOOPBACK,    // 127.0.0.0/8
         //       MULTICAST = Ipv4Address::MULTICAST,    // 224.0.0.0/4
         //       BROADCAST = Ipv4Address::BROADCAST,    // 255.255.255.255/32
         //       IETF = Ipv4Address::IETF,    // 192.0.0.0/24
         //       TEST_NET = Ipv4Address::TEST_NET,    // 192.0.2.0/24, 198.51.100.0/24, 203.0.113.0/24
         //       IPv6_TO_IPv4_RELAY = Ipv4Address::IPv6_TO_IPv4_RELAY,    // 192.88.99.0/24
         //       BENCHMARK = Ipv4Address::BENCHMARK,    // 198.18.0.0/15
         //       RESERVED = Ipv4Address::RESERVED,    // 240.0.0.0/4
         //       LINKLOCAL = Ipv4Address::LINKLOCAL,    // 169.254.0.0/16
         //       PRIVATE_NETWORK = Ipv4Address::PRIVATE_NETWORK,    // 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
         //       GLOBAL = Ipv4Address::GLOBAL
         //};
    };
}
#endif
