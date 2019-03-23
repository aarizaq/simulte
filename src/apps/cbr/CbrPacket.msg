//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

namespace inet;

//
// Packet used by the ~VoPSender and ~VoIPReceiver modules.
//

cplusplus {{
    #include <inet/common/packet/Packet.h>
}}

packet Packet;

class CbrPacket extends Packet
{
    unsigned int nframes;
    unsigned int IDframe;
    simtime_t timestamp;
    simtime_t arrivalTime;
    unsigned int size;
}
