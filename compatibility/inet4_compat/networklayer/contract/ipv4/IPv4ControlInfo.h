#ifndef INET_COMPAT_IPV4CONTROLINFO_H_
#define INET_COMPAT_IPV4CONTROLINFO_H_
#include <omnetpp.h>
#include <inet4_compat/networklayer/contract/ipv4/IPv4Address.h>

namespace inet {
    class IPv4Datagram;

    class IPv4ControlInfo : public ::omnetpp::cObject {
        public:
            IPv4Address getDestAddr() {return destAddr;}
            IPv4Address getSrcAddr() {return srcAddr;}
        private:
            IPv4Address destAddr;
            IPv4Address srcAddr;
            int interfaceId;
            bool multicastLoop;
            short protocol;
            unsigned char typeOfService;
            short timeToLive;
            bool dontFragment;
            IPv4Datagram* placeHolder;
    };
}
#endif
