//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//
package lte.epc;

import lte.epc.gtp.GtpUser;
import inet.linklayer.ppp.PppInterface;
import inet.node.base.NodeBase;
import inet.transportlayer.udp.Udp;

//
// Implements an EPC Serving Gateway (S-GW)
//
module SgwStandard extends NodeBase
{
    @display("bgb=920,462;i=device/server2");

    submodules:
        udp: Udp {
            @display("p=329,206");
        }
        gtp_user: GtpUser {
            @display("p=727,206");
            filter = false; // the sgw has no traffic flow filter
            tftFileName = "";
        }
    connections:
        udp.ipOut --> networkLayer.transportIn++;
        udp.ipIn <-- networkLayer.transportOut++;

        udp.appOut++ --> gtp_user.socketIn;
        gtp_user.socketOut --> udp.appIn++;
}
