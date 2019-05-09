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
import lte.epc.gtp.GtpUserSimplified;
import inet.linklayer.ppp.PppInterface;
import inet.node.base.NetworkLayerNodeBase;
import lte.epc.TrafficFlowFilter;
import inet.transportlayer.udp.Udp;

//
// Implements an EPC PDN Gateway (P-GW)
//
module PgwStandard extends NetworkLayerNodeBase
{
    parameters:
        string nodeType; // must be one between ENB and PGW
        @display("bgb=920,462;i=device/mainframe");
    gates:
        inout filterGate @labels(PPPFrame-conn);

    submodules:
        trafficFlowFilter: TrafficFlowFilter {
            ownerType = nodeType;
            @display("p=813,206");
        }
        pppInterface: PppInterface {
            @display("p=727,386");
        }
        udp: Udp {
            @display("p=329,206");
        }
        gtp_user: GtpUser {
            @display("p=591,206");
        }
    connections:
        //udp.ipOut --> networkLayer.transportIn++;
        //udp.ipIn <-- networkLayer.transportOut++;
        udp.ipOut --> networkLayer.transportIn;
        udp.ipIn <-- networkLayer.transportOut;

        pppInterface.upperLayerOut --> trafficFlowFilter.internetFilterGateIn;
        pppInterface.upperLayerIn <-- gtp_user.pppGate;
        pppInterface.phys <--> filterGate;

        //udp.appOut++ --> gtp_user.socketIn;
        //gtp_user.socketOut --> udp.appIn++;
        udp.appOut --> gtp_user.socketIn;
        gtp_user.socketOut --> udp.appIn;
        trafficFlowFilter.gtpUserGateOut --> gtp_user.trafficFlowFilterGate;
}

//
// Implements a simplified version of the EPC PDN Gateway (P-GW)
//
module PgwStandardSimplified extends NetworkLayerNodeBase
{
    parameters:
        string nodeType; // must be one between ENB and PGW
        @display("bgb=920,462;i=device/mainframe");
    gates:
        inout filterGate @labels(PPPFrame-conn);

    submodules:
        trafficFlowFilter: TrafficFlowFilterSimplified {
            ownerType = nodeType;
            @display("p=813,206");
        }
        pppInterface: PppInterface {
            @display("p=727,386");
        }
        udp: Udp {
            @display("p=329,206");
        }
        gtp_user: GtpUserSimplified {
            @display("p=591,206");
        }
    connections:
        //udp.ipOut --> networkLayer.transportIn++;
        //udp.ipIn <-- networkLayer.transportOut++;
        udp.ipOut --> ipv4.transportIn;
        udp.ipIn <-- ipv4.transportOut;

        pppInterface.upperLayerOut --> trafficFlowFilter.internetFilterGateIn;
        pppInterface.upperLayerIn <-- gtp_user.pppGate;
        pppInterface.phys <--> filterGate;

        //udp.appOut++ --> gtp_user.socketIn;
        //gtp_user.socketOut --> udp.appIn++;
        udp.appOut --> gtp_user.socketIn;
        gtp_user.socketOut --> udp.appIn;
        trafficFlowFilter.gtpUserGateOut --> gtp_user.trafficFlowFilterGate;
}
