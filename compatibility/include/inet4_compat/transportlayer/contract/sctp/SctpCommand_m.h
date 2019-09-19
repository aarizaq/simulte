#ifndef INET4_COMPAT_SCTPCOMMAND_M_H_
#define INET4_COMPAT_SCTPCOMMAND_M_H_
#include <inet/transportlayer/contract/sctp/SctpCommand_m.h>

namespace inet {
    class SCTPSimpleMessage : public SctpSimpleMessage {
        public:
            SCTPSimpleMessage(const char * c) : SctpSimpleMessage(c) {};
    };
}

#endif
