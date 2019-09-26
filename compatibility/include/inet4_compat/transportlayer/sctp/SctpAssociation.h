#ifndef INET4_COMPAT_SCTPASSOCIATION_H_
#define INET4_COMPAT_SCTPASSOCIATION_H_

#include <inet/transportlayer/sctp/SctpAssociation.h>

namespace inet {
    namespace sctp {
        class SCTPAssociation : public SctpAssociation {};
    }
}
#endif
