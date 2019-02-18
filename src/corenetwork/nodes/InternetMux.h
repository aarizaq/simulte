//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef _LTE_INTERNETMUX_H_
#define _LTE_INTERNETMUX_H_

#include <omnetpp.h>
#include "common/LteCommon.h"

class InternetMux : public inet::cSimpleModule
{
  protected:

    //* maps destination id to output gate.
    std::map<MacNodeId, inet::cGate*> routingTable_;

    inet::cGate* muxGate_[2];

    virtual void initialize() override;
    virtual void handleMessage(inet::cMessage *msg) override;

  public:

    InternetMux();

    void setRoutingEntry(const MacNodeId id, inet::cGate* gate)
    {
        routingTable_[id] = gate;
    }

    virtual ~InternetMux();
};

#endif
