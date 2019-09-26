SimuLTE
=======

LTE user plane simulation model, compatible with the INET Framework.
edit: (not currently)

Dependencies
------------

The current master/head version requires either of

once running, should work with OMNeT++ 5.4 and INET 4.0

Setup
-----

- PATH variable should include omnet bin directory and inet bin directory
- LIBRARY_PATH and LD_LIBRARY_PATH must include the location of the corresponding
shared object libraries (in theory LD_LIBRARY_PATH should be sufficient, 
since the build of both inet and simulte generate so files, however, it 
fixed an error for me)
- CPATH environment variable should include inet/4.0.0/src and 
simulte_inet4_compat/compatibility/include directory


Features
--------

General

- eNodeB and UE models
- Form-based configuration editor

PDCP-RRC

- Header compression/decompression
- Logical connection establishment  and maintenance 

RLC

- Multiplexing/Demultiplexing of MAC SDUs
- UM, (AM and TM testing) modes

MAC

- RLC PDUs buffering
- HARQ functionalities (with multi-codeword support)
- Allocation management
- AMC
- Scheduling Policies (MAX C/I, Proportional Fair, DRR)

PHY

- Heterogeneous Net (HetNets) support: Macro, micro, pico eNbs
- Channel Feedback management
- Dummy channel model
- Realistic channel model with
  - cell interference
  - path-loss
  - fast fading
  - shadowing 
  - (an-isotropic antennas - work in progress)

Other

- X2 communication support
- (Relay support - work in progress)
- Distributed Antenna System - DAS (Testing)

Applications

- VoIP
- Gaming
- Trace Based traffic


Limitations
-----------

- User Plane only (Control Plane not modeled)
- FDD only (TDD not supported)
- no EPS bearer support – note: a similar concept, "connections", has 
  been implemented, but they are neither dynamic nor statically 
  configurable via some config file
- radio bearers not implemented, not even statically configured radio 
  bearers (dynamically allocating bearers would need the RRC protocol, 
  which is Control Plane so not implemented)
- handovers not implemented (no X2-based handover, that is; S1-based 
  handover would require an S-GW model)


Using the configuration editor
------------------------------

(edit: not sure about that)

SimuLTE also contains a form-based configuration editor to edit the most 
often used simulation parameters. To use it, right-click an Ini file, and choose
'Open With | SimuLTE Configuration Editor' from the context menu. Try it on 
'simulations/demo/gui.ini'.
